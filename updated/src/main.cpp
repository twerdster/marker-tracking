// Modern image/model/replay frontend. Original tree implementation: SearchTree.cpp.
#include "SearchTree.h"
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <set>
namespace fs=std::filesystem;
using Clock=std::chrono::steady_clock;
struct Result {
    bool found=false;
    int candidates=0,matches=0,inliers=0;
    double error=-1,ms=0;
    cv::Mat H;
    std::vector<cv::Point2f> from,to,corners;
    std::vector<uchar> mask;
};
cv::Mat gray(const cv::Mat& image) {
    if(image.empty()) throw std::runtime_error("Empty image");
    cv::Mat out;
    if(image.channels()==1) out=image; else cv::cvtColor(image,out,cv::COLOR_BGR2GRAY);
    return out;
}
std::vector<cv::KeyPoint> detect(const cv::Mat& image) {
    // ORB's FAST pyramid and intensity-centroid orientation; no ORB descriptors.
    auto orb=cv::ORB::create(900,1.2f,5,34,0,2,cv::ORB::HARRIS_SCORE,31,12);
    std::vector<cv::KeyPoint> points;
    orb->detect(image,points);
    std::sort(points.begin(),points.end(),[](const auto& a,const auto& b){return a.response>b.response;});
    return points;
}
cv::Mat patch(const cv::Mat& image,const cv::KeyPoint& p,double angle=0,double scale=1,double dx=0,double dy=0) {
    const double a=(p.angle+angle)*CV_PI/180., s=(p.size/31.)*scale;
    const double c=cos(a)*s, sn=sin(a)*s;
    cv::Mat transform=(cv::Mat_<double>(2,3)<<c,-sn,p.pt.x+dx-32*c+32*sn,sn,c,p.pt.y+dy-32*sn-32*c);
    cv::Mat out;
    cv::warpAffine(image,out,transform,cv::Size(WIND,WIND),cv::INTER_LINEAR|cv::WARP_INVERSE_MAP,cv::BORDER_REFLECT_101);
    return out;
}
struct Model {
    cv::Size size;
    cv::Mat marker;
    std::vector<cv::KeyPoint> points;
    std::vector<std::unique_ptr<SearchTree>> trees;
    void train(const cv::Mat& image,int count=32,int augments=96) {
        marker=image.clone(); size=image.size(); auto g=gray(image);
        auto all=detect(g);
        for(const auto& p:all) {
            bool near=false;
            for(const auto& chosen:points) if(cv::norm(p.pt-chosen.pt)<9) near=true;
            if(!near) points.push_back(p);
            if(points.size()==120) break;
        }
        if(points.size()<8) throw std::runtime_error("Marker needs at least 8 separated texture features");
        const int width=int(points.size()), samples=width*augments;
        std::vector<double> buffer(size_t(samples)*WIND*WIND);
        std::vector<int> indices(samples); std::iota(indices.begin(),indices.end(),0);
        cv::RNG rng(20261114);
        for(int row=0;row<augments;++row) for(int col=0;col<width;++col) {
            auto p=patch(g,points[col],row? rng.uniform(-17.,17.):0,
                         row? rng.uniform(0.82,1.22):1,
                         row? rng.uniform(-1.8,1.8):0,row? rng.uniform(-1.8,1.8):0);
            double gain=row?rng.uniform(0.75,1.25):1, bias=row?rng.uniform(-18.,18.):0;
            for(int y=0;y<WIND;++y) for(int x=0;x<WIND;++x)
                buffer[size_t(row)*WIND*WIND*width+size_t(y)*WIND*width+col*WIND+x]=cv::saturate_cast<uchar>(p.at<uchar>(y,x)*gain+bias);
        }
        srand(42);
        for(int i=0;i<count;++i) {
            auto tree=std::make_unique<SearchTree>();
            tree->BuildTree(indices,buffer.data(),width,augments);
            tree->CreateNonZeroIndicesAndNormalize(); trees.push_back(std::move(tree));
        }
    }
    void save(const fs::path& dir) {
        if(fs::exists(dir)) throw std::runtime_error("Model output already exists; choose a new directory");
        fs::create_directories(dir);
        cv::FileStorage file((dir/"model.yml").string(),cv::FileStorage::WRITE);
        file<<"format"<<1<<"width"<<size.width<<"height"<<size.height<<"trees"<<int(trees.size());
        cv::write(file,"points",points); file.release();
        cv::imwrite((dir/"marker.png").string(),marker);
        for(size_t i=0;i<trees.size();++i) {auto path=(dir/("tree"+std::to_string(i)+".dat")).string(); trees[i]->Write(path.data());}
    }
    void load(const fs::path& dir) {
        auto meta=dir/"model.yml";
        if(!fs::is_regular_file(meta)||fs::file_size(meta)>1024*1024) throw std::runtime_error("Missing or oversized model metadata");
        cv::FileStorage file(meta.string(),cv::FileStorage::READ);
        int count=0,format=0; file["format"]>>format; file["width"]>>size.width; file["height"]>>size.height; file["trees"]>>count;
        cv::read(file["points"],points);
        if(format!=1||count<1||count>100||points.size()<8||points.size()>120||size.width<68||size.height<68||size.width>10000||size.height>10000) throw std::runtime_error("Invalid model metadata");
        for(const auto& p:points) if(!std::isfinite(p.pt.x)||!std::isfinite(p.pt.y)||p.pt.x<0||p.pt.y<0||p.pt.x>=size.width||p.pt.y>=size.height) throw std::runtime_error("Invalid marker point");
        marker=cv::imread((dir/"marker.png").string());
        if(marker.empty()||marker.size()!=size) throw std::runtime_error("Missing marker image");
        for(int i=0;i<count;++i) {auto tree=std::make_unique<SearchTree>(); auto path=(dir/("tree"+std::to_string(i)+".dat")).string(); tree->Read(path.data()); trees.push_back(std::move(tree));}
    }
    Result track(const cv::Mat& image) {
        auto start=Clock::now(); Result result; auto g=gray(image); auto detected=detect(g); result.candidates=int(detected.size());
        struct Match {int label; double score; cv::Point2f pt;}; std::vector<Match> proposed;
        for(const auto& p:detected) for(double queryScale:{0.5,0.70710678,1.,1.41421356,2.}) {
            auto sample=patch(g,p,0,queryScale); unsigned votes[MAX_CLASSIFIERS_NUM]={};
            for(auto& tree:trees) tree->Find(sample.data,WIND,WIND,0,votes,1);
            int best=-1; unsigned first=0,second=0;
            for(size_t label=0;label<points.size();++label) {
                if(votes[label]>first) {second=first;first=votes[label];best=int(label);}
                else second=std::max(second,votes[label]);
            }
            if(best>=0&&first>0.10*trees.size()*1024&&first>second*1.80) proposed.push_back({best,double(first),p.pt});
        }
        std::sort(proposed.begin(),proposed.end(),[](const auto& a,const auto& b){return a.score>b.score;});
        std::set<int> used;
        for(const auto& m:proposed) if(used.insert(m.label).second) {result.from.push_back(points[m.label].pt); result.to.push_back(m.pt);}
        result.matches=int(result.from.size());
        if(result.matches>=8) {
            cv::setRNGSeed(42);
            result.H=cv::findHomography(result.from,result.to,cv::RANSAC,3.,result.mask,3000,0.995);
            result.inliers=cv::countNonZero(result.mask);
            if(!result.H.empty()&&cv::checkRange(result.H)&&result.inliers>=8&&result.inliers>=result.matches*0.35) {
                std::vector<cv::Point2f> corners={{0,0},{float(size.width-1),0},{float(size.width-1),float(size.height-1)},{0,float(size.height-1)}};
                cv::perspectiveTransform(corners,result.corners,result.H);
                const double area=std::abs(cv::contourArea(result.corners));
                bool plausible=cv::isContourConvex(result.corners)&&area>=400&&area<image.total()*1.8;
                for(const auto& c:result.corners) plausible=plausible&&std::isfinite(c.x)&&std::isfinite(c.y)&&c.x>-image.cols&&c.x<2*image.cols&&c.y>-image.rows&&c.y<2*image.rows;
                if(plausible) {
                    std::vector<cv::Point2f> fitted; cv::perspectiveTransform(result.from,fitted,result.H);
                    double squared=0; for(size_t k=0;k<fitted.size();++k) if(result.mask[k]) squared+=cv::norm(fitted[k]-result.to[k])*cv::norm(fitted[k]-result.to[k]);
                    result.error=sqrt(squared/result.inliers); result.found=true;
                }
            }
        }
        result.ms=std::chrono::duration<double,std::milli>(Clock::now()-start).count(); return result;
    }
    cv::Mat render(const cv::Mat& image,const Result& r) {
        cv::Mat out; if(image.channels()==1) cv::cvtColor(image,out,cv::COLOR_GRAY2BGR); else out=image.clone();
        if(r.found) {
            std::vector<cv::Point> corners; for(auto c:r.corners) corners.emplace_back(cvRound(c.x),cvRound(c.y));
            cv::polylines(out,corners,true,cv::Scalar(60,230,80),2,cv::LINE_AA);
            for(size_t k=0;k<r.to.size();++k) if(r.mask[k]) cv::circle(out,r.to[k],3,cv::Scalar(40,220,255),-1,cv::LINE_AA);
        }
        const std::string status=r.found?"TREE MATCH: "+std::to_string(r.inliers)+" inliers":"NOT FOUND";
        cv::rectangle(out,{0,0,out.cols,32},cv::Scalar(25,25,25),-1);
        cv::putText(out,status,{10,22},cv::FONT_HERSHEY_SIMPLEX,0.52,r.found?cv::Scalar(60,230,80):cv::Scalar(80,170,255),1,cv::LINE_AA);
        return out;
    }
};
void writeResult(cv::FileStorage& f,const Result& r,int frame=0) {
    f<<"{"<<"frame"<<frame<<"found"<<int(r.found)<<"candidates"<<r.candidates<<"matches"<<r.matches<<"inliers"<<r.inliers<<"inlier_rmse_pixels"<<r.error<<"runtime_ms"<<r.ms;
    if(r.found) f<<"homography"<<r.H<<"corners"<<r.corners;
    f<<"}";
}
void demo(Model& model,const fs::path& out) {
    if(fs::exists(out)) throw std::runtime_error("Demo output exists; choose a new directory");
    fs::create_directories(out);
    cv::VideoWriter video((out/"replay.avi").string(),cv::CAP_OPENCV_MJPEG,cv::VideoWriter::fourcc('M','J','P','G'),12,{720,480});
    cv::VideoWriter raw((out/"input.avi").string(),cv::CAP_OPENCV_MJPEG,cv::VideoWriter::fourcc('M','J','P','G'),12,{720,480});
    if(!video.isOpened()||!raw.isOpened()) throw std::runtime_error("Cannot create demo movie");
    cv::FileStorage results((out/"metrics.json").string(),cv::FileStorage::WRITE|cv::FileStorage::FORMAT_JSON);
    results<<"fixture"<<"synthetic perspective/occlusion/absence replay of a real marker photograph"<<"frames"<<"[";
    std::vector<cv::Point2f> source={{0,0},{float(model.size.width-1),0},{float(model.size.width-1),float(model.size.height-1)},{0,float(model.size.height-1)}};
    int present=0,found=0,absent=0,falsePositive=0;double squared=0;int measured=0;
    for(int frame=0;frame<60;++frame) {
        bool expected=frame<36||frame>=48;
        double a=frame*0.1;
        cv::Mat image(480,720,CV_8UC3,cv::Scalar(52,47,42));
        // Fixed background line grid is unrelated to the photographed target.
        for(int x=0;x<720;x+=45) cv::line(image,{x,32},{x,479},cv::Scalar(63,58,53));
        for(int y=45;y<480;y+=45) cv::line(image,{0,y},{719,y},cv::Scalar(63,58,53));
        std::vector<cv::Point2f> target={{float(160+35*sin(a)),float(90+15*cos(a))},{float(540+20*cos(a)),float(100+25*sin(a))},{float(525+30*sin(a)),float(390+12*cos(a))},{float(175+12*cos(a)),float(375+25*sin(a))}};
        cv::Mat H=cv::getPerspectiveTransform(source,target);
        if(expected) {
            cv::Mat warped,mask(model.size,CV_8U,cv::Scalar(255)),warpedMask;
            cv::warpPerspective(model.marker,warped,H,image.size());cv::warpPerspective(mask,warpedMask,H,image.size());warped.copyTo(image,warpedMask);
            if(frame>=24&&frame<36) cv::rectangle(image,{340,120,200,260},cv::Scalar(90,80,70),-1);
        }
        raw.write(image);
        auto r=model.track(image); auto visual=model.render(image,r);
        cv::putText(visual,"Synthetic replay | original-derived classification trees",{12,460},cv::FONT_HERSHEY_SIMPLEX,.48,{230,230,230},1,cv::LINE_AA);
        video.write(visual);
        if(frame==10||frame==28||frame==42||frame==54) cv::imwrite((out/("frame-"+std::to_string(frame)+".png")).string(),visual);
        double cornerError=-1;
        if(expected) {++present;if(r.found) {++found;cornerError=0;for(int k=0;k<4;++k) cornerError+=cv::norm(r.corners[k]-target[k])*cv::norm(r.corners[k]-target[k]);cornerError=sqrt(cornerError/4);squared+=cornerError*cornerError;++measured;}}
        else {++absent;if(r.found)++falsePositive;}
        results<<"{"<<"frame"<<frame<<"expected_present"<<int(expected)<<"found"<<int(r.found)<<"matches"<<r.matches<<"inliers"<<r.inliers<<"corner_rmse_pixels"<<cornerError<<"runtime_ms"<<r.ms<<"}";
    }
    results<<"]"<<"present_frames"<<present<<"found_present_frames"<<found<<"absent_frames"<<absent<<"false_positive_frames"<<falsePositive<<"corner_rmse_found_frames_pixels"<<(measured?sqrt(squared/measured):-1.);
    std::cout<<"Synthetic replay: "<<found<<"/"<<present<<" present frames detected; "<<falsePositive<<"/"<<absent<<" absent-frame false positives\n";
}
void require(bool ok,const std::string& message) {if(!ok) throw std::runtime_error("Selftest: "+message);}
int main(int argc,char** argv) {
    try {
        if(argc<2) throw std::runtime_error("Usage: marker_tracker train MARKER MODEL_DIR | image MODEL_DIR INPUT OUTPUT.png RESULT.json | video MODEL_DIR INPUT OUTPUT.avi RESULT.json | camera MODEL_DIR [INDEX] | demo MODEL_DIR NEW_OUTPUT_DIR | selftest");
        const std::string command=argv[1];
        if(command=="train"&&argc==4) {
            Model model; auto start=Clock::now(); model.train(cv::imread(argv[2])); model.save(argv[3]);
            std::cout<<"Trained "<<model.trees.size()<<" original-derived trees for "<<model.points.size()<<" marker points in "<<std::chrono::duration<double>(Clock::now()-start).count()<<" seconds\n";
        } else if(command=="image"&&argc==6) {
            Model model; model.load(argv[2]); auto image=cv::imread(argv[3]); auto r=model.track(image); cv::imwrite(argv[4],model.render(image,r));
            cv::FileStorage f(argv[5],cv::FileStorage::WRITE|cv::FileStorage::FORMAT_JSON); f<<"frames"<<"["; writeResult(f,r); f<<"]";
            std::cout<<(r.found?"FOUND ":"NOT FOUND ")<<r.inliers<<" inliers, "<<r.error<<" pixel inlier RMSE, "<<r.ms<<" ms\n";
        } else if((command=="video"&&argc==6)||(command=="camera"&&(argc==3||argc==4))) {
            const bool camera=command=="camera"; Model model; model.load(argv[2]);
            cv::VideoCapture capture; if(camera) capture.open(argc==4?std::stoi(argv[3]):0); else capture.open(argv[3]);
            if(!capture.isOpened()) throw std::runtime_error("Cannot open input video/camera");
            cv::VideoWriter writer; cv::FileStorage f;
            if(!camera) {f.open(argv[5],cv::FileStorage::WRITE|cv::FileStorage::FORMAT_JSON);f<<"frames"<<"[";}
            int frame=0; cv::Mat image;
            while(capture.read(image)) {
                auto r=model.track(image); auto vis=model.render(image,r);
                if(camera) {cv::imshow("Marker Tracking - updated tree port (Esc closes)",vis); if(cv::waitKey(1)==27) break;}
                else {
                    if(!writer.isOpened()) {double fps=capture.get(cv::CAP_PROP_FPS); if(!std::isfinite(fps)||fps<1||fps>240) fps=15;writer.open(argv[4],cv::CAP_OPENCV_MJPEG,cv::VideoWriter::fourcc('M','J','P','G'),fps,image.size()); if(!writer.isOpened()) throw std::runtime_error("Cannot create output video");}
                    writer.write(vis); writeResult(f,r,frame);
                }
                ++frame;
            }
            if(!camera) f<<"]";
            if(frame==0) throw std::runtime_error("No input frames decoded");
            std::cout<<"Processed "<<frame<<" frames\n";
        } else if(command=="demo"&&argc==4) {
            Model model;model.load(argv[2]);demo(model,argv[3]);
        } else if(command=="selftest"&&argc==2) {
            // Real algorithm regression: constant patches previously left split coordinates uninitialized.
            const int width=8,rows=8; std::vector<double> samples(width*rows*WIND*WIND,100.); std::vector<int> ids(width*rows); std::iota(ids.begin(),ids.end(),0);
            SearchTree tree; tree.BuildTree(ids,samples.data(),width,rows); tree.CreateNonZeroIndicesAndNormalize();
            cv::Mat p(WIND,WIND,CV_8U,cv::Scalar(100)); unsigned votes[MAX_CLASSIFIERS_NUM]={}; tree.Find(p.data,WIND,WIND,0,votes,1);
            for(int k=0;k<width;++k) require(votes[k]==128,"constant-patch leaf histogram");
            unsigned total=0;for(auto v:votes) total+=v; require(total==1024,"histogram normalized");
            auto dir=fs::temp_directory_path()/("marker-tree-test-"+std::to_string(Clock::now().time_since_epoch().count()));fs::create_directories(dir);
            auto path=(dir/"tree.dat").string(); tree.Write(path.data()); SearchTree read;read.Read(path.data());
            unsigned again[MAX_CLASSIFIERS_NUM]={};read.Find(p.data,WIND,WIND,0,again,1);require(std::equal(std::begin(votes),std::end(votes),std::begin(again)),"tree serialization votes");
            int rejected=0; for(const auto& bad:std::vector<std::string>{"","-1 0 0 0 1", "0 0 0 0 301", "0 0 0 0 1 900 1 ( ) ( ) ( )", "0 0 0 0 1 0 1 ( ) ( ) ( ) trailing"}) {
                std::ofstream(path)<<bad; try {SearchTree t;t.Read(path.data());} catch(const std::exception&) {++rejected;}
            }
            require(rejected==5,"malformed trees rejected");fs::remove_all(dir);
            std::cout<<"PASS: constant split fallback, 8 histogram classes, normalized sum, serialization equivalence, 5 malformed tree rejection cases\n";
        } else throw std::runtime_error("Unknown command or argument count");
        return 0;
    } catch(const std::exception& e) {std::cerr<<"Error: "<<e.what()<<"\n";return 1;}
}
