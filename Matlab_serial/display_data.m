global serial_data
plot_data_1=1;
plot_data_2=2;
plot_data_3=4;
% plot_data_4=4;

fig_1=figure;
fig_1.Position=[12 243 560 420];
axes_1=axes;


fig_2=figure;
fig_2.Position=[591 246 581 420];
axes_2=axes;
% 
plot(axes_1,serial_data{plot_data_1}(:,1),serial_data{plot_data_1}(:,2))

plot(axes_2,...
    serial_data{plot_data_3}(:,1),serial_data{plot_data_3}(:,2),'g',...
    serial_data{plot_data_2}(:,1),serial_data{plot_data_2}(:,2),'b')
% plot(axes_2,...
%     serial_data{plot_data_2}(:,1),serial_data{plot_data_2}(:,2)-serial_data{plot_data_3}(:,2))
% plot(axes_3,serial_data{plot_data_3}(:,1),serial_data{plot_data_3}(:,2))
% plot(axes_4,serial_data{plot_data_4}(:,1),serial_data{plot_data_4}(:,2))




