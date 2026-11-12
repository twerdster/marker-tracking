function [res] = ...
    random_transformation(theta_bound,phi_bound,lambda_min,lambda_max,t_bound)

lambda1 = random('unif',lambda_min,lambda_max);
lambda2 = random('unif',lambda_min,lambda_max);
lambda = [lambda1 lambda2];
S = maketform('affine',[lambda1 0 0;0 lambda2 0;0 0 1]);

theta = random('unif',-theta_bound,theta_bound);
phi = random('unif',-phi_bound,phi_bound);

%create matrix as composite A = R_theta*R_phi_inv*S*R_phi
R_phi = maketform('affine',...
[cos(phi) sin(phi) 0; -sin(phi) cos(phi) 0;0 0 1]);
R_phi_inv = maketform('affine',...
[cos(phi) -sin(phi) 0; sin(phi) cos(phi) 0;0 0 1]);
R_theta = maketform('affine',...
[cos(theta) sin(theta) 0; -sin(theta) cos(theta) 0;0 0 1]);
A =maketform('composite',[R_theta R_phi_inv S R_phi]);

tx = randi([-t_bound,t_bound],1,1);
ty = randi([-t_bound,t_bound],1,1);
t = [tx;ty]; % pixels of translation-t_x,t_y
res.transform = A;
res.theta = theta;
res.phi = phi;
res.lambda = lambda;
res.t =t;

%x = 50; y = 70; % example - will be coordinates of feature point
%K = I(y-40:y+40,x-40:x+40);
%tranK = imtransform(K,A);
% TODO - size(tranK) -> center of image -> return view patch
