% /*
% * forcefield_debug.m - modelling forcefield net forces, using linear
% programming solver - version 2
% * Created by: David Baker
% *         on: 01/25/15
% * All code available at https://github.com/colinfeo/UPennSeniorDesign
% *  - Last backed up 05/10/15
% * Revisions:
% *  - see github repo for full list of commits
% *  - Revised most recently by: David Baker
% *  - Revised most recently on: 02/10/15
% *  - Description:
% *     Added animation to visualize force outputs.
% *
% */

% animation loop
figure
while 1
    % cm
    B = 64.60;
    A = 45.24;

    separation = 5;
    
    % ranges in x from 1-1680, y from 1-1050
    mouse_position = get(0,'PointerLocation'); 

    % these will be given to the driver...
    EE_bot_pos = [B*(1680-mouse_position(1))/1680; A*mouse_position(2)/1050-separation/2];
    EE_top_pos = [B*(1680-mouse_position(1))/1680; A*mouse_position(2)/1050+separation/2];

%     EE_mid_pos = [(EE_bot_pos(1)+EE_top_pos(1))/2;((EE_bot_pos(2)+EE_top_pos(2))/2];
    
    NW = [B;A];
    SW = [B;0];
    NE = [0;A];
    SE = [0;0];
    
    L0 = norm(EE_bot_pos - SE)
    L1 = norm(EE_top_pos - NE)
    L2 = norm(EE_top_pos - NW)
    L3 = norm(EE_bot_pos - SW)

    
    v0 = (EE_bot_pos - SE)/norm(EE_bot_pos - SE);
    v1 = (EE_top_pos - NE)/norm(EE_top_pos - NE);
    v2 = (EE_top_pos - NW)/norm(EE_top_pos - NW);
    v3 = (EE_bot_pos - SW)/norm(EE_bot_pos - SW);

    % desired force (given by Unity program)
    fx = -3;
    fy = 4;
    
    % moment calculations
    y_arm = (EE_bot_pos(2)-EE_top_pos(2))/2;
    x_arm = (EE_bot_pos(1)-EE_top_pos(1))/2;
    % coefficients
    c0_coeff = -1*v0(1)*y_arm + v0(2)*(x_arm);
    c1_coeff = v1(1)*y_arm + -1*v1(2)*(x_arm);
    c2_coeff = v2(1)*y_arm + -1*v2(2)*(x_arm);
    c3_coeff = -1*v3(1)*y_arm + v3(2)*(x_arm);

    desired_moment = -0.3;

    % find x that minimizes f(x) = abs(c0)+c1+c2+c3
    f = [-1; -1; -1; -1];
    % v0(1)*c0 +v1(1)*c1 + = fx

    % matrix and vector for equality constraints
    Aeq = [v0(1) v1(1) v2(1) v3(1)
           v0(2) v1(2) v2(2) v3(2)
           c0_coeff c1_coeff c2_coeff c3_coeff];
    beq = [fx; fy; desired_moment];

% upper and lower bound constraints
    lb = -10*ones(4,1);
    ub = -1*ones(4,1);
tic
    c = linprog(f,[],[],Aeq,beq,lb,ub)
toc
    
    % draw field
    field_x = [0 0 B B];
    field_y = [0 A A 0];
    fill(field_x,field_y,[224/255 255/255 255/255])
    
    box_x = [8.565 8.565 56.0376 56.0376];
    box_y = [8.575 36.665 36.665 8.575];
    fill(box_x, box_y, 'y')
    hold on
    % draw position
    plot(EE_bot_pos(1),EE_bot_pos(2),'*');
    plot(EE_top_pos(1),EE_top_pos(2),'o');
    axis([-1 B+1 -1 A+1]) % to fit display
    hold on
    % draw cables
    plot([0 EE_bot_pos(1)],[0 EE_bot_pos(2)],'k','Linewidth',2)
    plot([B EE_bot_pos(1)],[0 EE_bot_pos(2)],'k','Linewidth',2)  
    plot([0 EE_top_pos(1)],[A EE_top_pos(2)],'k','Linewidth',2)
    plot([B EE_top_pos(1)],[A EE_top_pos(2)],'k','Linewidth',2)  
%     
%     plot([1280 mouse_position(1)],[800 mouse_position(2)],'k','Linewidth',2)
%     plot([1280 mouse_position(1)],[0 mouse_position(2)],'k','Linewidth',2)
%     plot([0 mouse_position(1)],[0 mouse_position(2)],'k','Linewidth',2)

    quiver(EE_bot_pos(1),EE_bot_pos(2),c(1)*v0(1),c(1)*v0(2),2,'r','linewidth',3);
    quiver(EE_bot_pos(1),EE_bot_pos(2),c(4)*v3(1),c(4)*v3(2),2,'r','linewidth',3);
    quiver(EE_top_pos(1),EE_top_pos(2),c(2)*v1(1),c(2)*v1(2),2,'r','linewidth',3);
    quiver(EE_top_pos(1),EE_top_pos(2),c(3)*v2(1),c(3)*v2(2),2,'r','linewidth',3);

    bottom__net_force = [c(1)*v0(1)+c(4)*v3(1);c(1)*v0(2)+c(4)*v3(2)];
    quiver(EE_bot_pos(1),EE_bot_pos(2),bottom__net_force(1),bottom__net_force(2),2,'b','linewidth',3);
    
    top_net_force = [c(2)*v1(1)+c(3)*v2(1);c(2)*v1(2)+c(3)*v2(2)];
    quiver(EE_top_pos(1),EE_top_pos(2),top_net_force(1),top_net_force(2),2,'b','linewidth',3);
    
    total_net_force = bottom__net_force + top_net_force;
    quiver((EE_top_pos(1)+EE_bot_pos(1))/2,(EE_top_pos(2)+EE_bot_pos(2))/2,total_net_force(1),total_net_force(2),2,'g','linewidth',3);

    
    hold off
   drawnow
end

fclose(m2);