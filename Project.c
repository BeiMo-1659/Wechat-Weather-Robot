#include <stdio.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <strings.h>
#include <sys/mman.h>

#include <linux/input.h>
#include "lcd.h"
#include "api_v4l2.h"
FrameBuffer freambuf;
int tx,ty;
 /*显示图片*/
int showbmp(char *filename,int x0,int y0,int w,int h)
 {
	 //1.打开lcd屏幕驱动
          int lcd_fd=open("/dev/fb0",O_RDWR);
          if(lcd_fd<0)
          {
		    printf(" open lcd fail\n");	  
			  
		  }			  
		  printf("open lcd ok\n"); 
		 // 2.申请虚拟内存
		 char *addr=mmap(NULL,800*480*4,PROT_READ| PROT_WRITE,MAP_SHARED,lcd_fd,0);
		// addr[800*480*4]
		 
		 // 3.打开bmp图片
         int bmp_fd=open(filename,O_RDWR);
		 if(bmp_fd<0)
		 {
			 
			printf("open bmp fail\n"); 
		 }
		 else
	     printf("open bmp ok\n");
		 
		 //4.读取bmp图片数据
		 char bmp_buf[w*h*3];
		 bzero(bmp_buf,w*h*3);
		 //跳过头54个字节不读 移动光标
		 lseek(bmp_fd,54,SEEK_SET);
		 
		 read(bmp_fd,bmp_buf,w*h*3);
		 
		 
		 //5.把24位bmp数据转换成32位lcd数据并且放到虚拟内存里
		 int x,y;//x 列  y 行
		 for(y=y0;y<y0+h;y++)
		  for(x=x0;x<x0+w;x++)
	      {
			//第y行第x列
  
			addr[0+4*x+4*y*800]=bmp_buf[0+3*(x-x0)+3*w*(h-1-(y-y0))];
			addr[1+4*x+4*y*800]=bmp_buf[1+3*(x-x0)+3*w*(h-1-(y-y0))];
			addr[2+4*x+4*y*800]=bmp_buf[2+3*(x-x0)+3*w*(h-1-(y-y0))];
		    addr[3+4*x+4*y*800]=0;  
			  
		  }

		//6.释放虚拟内存，关闭lcd驱动 bmp
		munmap(addr,800*480*4);
		close(lcd_fd);
		close(bmp_fd); 	 
	 
 }
 /*图片特效*/
 int T_showbmp(char *filename,int x0,int y0,int w,int h)
 {
	 //1.打开lcd屏幕驱动
          int lcd_fd=open("/dev/fb0",O_RDWR);
          if(lcd_fd<0)
          {
		    printf(" open lcd fail\n");	  
			  
		  }			  
		  printf("open lcd ok\n"); 
		 // 2.申请虚拟内存
		 char *addr=mmap(NULL,800*480*4,PROT_READ| PROT_WRITE,MAP_SHARED,lcd_fd,0);
		// addr[800*480*4]
		 
		 // 3.打开bmp图片
         int bmp_fd=open(filename,O_RDWR);
		 if(bmp_fd<0)
		 {
			 
			printf("open bmp fail\n"); 
		 }
		 else
	     printf("open bmp ok\n");
		 
		 //4.读取bmp图片数据
		 char bmp_buf[w*h*3];
		 bzero(bmp_buf,w*h*3);
		 //跳过头54个字节不读 移动光标
		 lseek(bmp_fd,54,SEEK_SET);
		 
		 read(bmp_fd,bmp_buf,w*h*3);
		 
		 
		 //5.把24位bmp数据转换成32位lcd数据并且放到虚拟内存里
		 int x,y;//x 列  y 行
		 for(y=y0;y<y0+h;y++){
		  for(x=x0;x<x0+w;x++)
	      {
			//第y行第x列
  
			addr[0+4*x+4*y*800]=bmp_buf[0+3*(x-x0)+3*w*(h-1-(y-y0))];
			addr[1+4*x+4*y*800]=bmp_buf[1+3*(x-x0)+3*w*(h-1-(y-y0))];
			addr[2+4*x+4*y*800]=bmp_buf[2+3*(x-x0)+3*w*(h-1-(y-y0))];
		    addr[3+4*x+4*y*800]=0;  
			  
		  }
		  usleep(1000);
		 }
		//6.释放虚拟内存，关闭lcd驱动 bmp
		munmap(addr,800*480*4);
		close(lcd_fd);
		close(bmp_fd); 	 
	 
 }

//获取触摸坐标
int get_xy()
{
	
  	
  //1.打开触摸屏驱动
  int ts_fd=open("/dev/input/event0",O_RDWR);
  if(ts_fd<0)
  {
	  
	  printf("open ts fail\n");
  }
  else
	  printf("open ts ok\n");
  
  struct input_event ts;
   
  //4.分析输入子系统获取触摸信息
  while(1)
  {
	 read(ts_fd,&ts,sizeof(ts)); 
	if(ts.type==EV_ABS)  
	{
		if(ts.code==ABS_X)
		{
			tx=ts.value;
			
		}
		if(ts.code==ABS_Y)
		{
			ty=ts.value;
			
		}
	} 
	if(ts.type==EV_KEY&&ts.code==BTN_TOUCH)
    {
	  if(ts.value==0)	
	  break;	
		
	}		
	  

	  
  }
  
  //5.关闭触摸屏驱动
  close(ts_fd);
	
	
	
}
/*相册功能*/
int photo()
{   
	char bmpname[8][32]={
	"/xiangce/1.bmp",
    "/xiangce/2.bmp",
    "/xiangce/3.bmp",
    "/xiangce/4.bmp",
	"/xiangce/5.bmp",
    "/xiangce/6.bmp",
    "/xiangce/7.bmp",
    "/xiangce/8.bmp",	
	 
 };//bmpname[0]  0.bmp
  //显示第一张图片	或者  点击区域的提示图
   showbmp(bmpname[0],80,48,640,384);
   showbmp("/system_bmp/zuo.bmp",0,130,80,100);
   showbmp("/system_bmp/you.bmp",720,130,80,100);
   showbmp("/system_bmp/tui.bmp",720,0,80,80);
  //循环获取xy坐标，判断相应功能
   int i=1;
  while(1){
 r:	get_xy();
    if(tx>720&&tx<=800&&ty>=130&&ty<230)//下一张
	{   
		T_showbmp(bmpname[i++],80,48,640,384);
		if(i>=8){
		  i=0; 
		  goto r;
		//showbmp(bmpname[i++],80,48,640,384);
	 }		 	
	}
	
	if(tx>=0&&tx<80&&ty>=130&&ty<230)//上一张
	{  if(i<=0){
		i=8; 
        //i--;
		//showbmp(bmpname[i--],80,48,640,384);
	 }
		i--; 	 
	  T_showbmp(bmpname[i--],80,48,640,384);
		
     	
	}	
    if(tx>720&&tx<=800&&ty>=0&&ty<48)//退出
	{
	  break;	
	}	
  
  }	
}
/*音乐功能*/
int music(){
	int i;
	char yinyue[4][128]={
		"mplayer -slave -quiet -input  file=/pipe -geometry  0:0 -zoom -x 800 -y 440  /yinyue/1.mp3 &",
        "mplayer -slave -quiet -input  file=/pipe -geometry  0:0 -zoom -x 800 -y 440  /yinyue/2.mp3 &",
        "mplayer -slave -quiet -input  file=/pipe -geometry  0:0 -zoom -x 800 -y 440  /yinyue/3.mp3 &",
		"mplayer -slave -quiet -input  file=/pipe -geometry  0:0 -zoom -x 800 -y 440  /yinyue/4.mp3 &",

	};
	showbmp("/system_bmp/system_yinyue/m_beijing.bmp",49,49,700,380);//显示背景
	showbmp("/system_bmp/system_shipin/v_kuaitui.bmp",125,430,50,50);//上一首
	showbmp("/system_bmp/system_yinyue/m_yinliang-.bmp",225,430,50,50);//音量-
	showbmp("/system_bmp/system_yinyue/m_tui.bmp",325,430,50,50);      //退出
	showbmp("/system_bmp/system_yinyue/m_zanting.bmp",425,430,50,50);//暂停
	showbmp("/system_bmp/system_yinyue/m_yinliang+.bmp",525,430,50,50);//音量+
	showbmp("/system_bmp/system_yinyue/m_xiayishou.bmp",625,430,50,50);//下一首
	 
	 system("rm /pipe");
	 system("mkfifo /pipe");
	 system("chmod 777 pipe");
	 system(yinyue[0]);
	 i=1;
	 while(1)
     {
		get_xy();
		 if(tx>=125&&tx<175&&ty>=430&&ty<=480)//上一首
		{        if(i<0){
					 i=3;
				 }
	       		 system("killall -kill mplayer");
				 system(yinyue[i--]);
				 
		}
		 if(tx>=225&&tx<275&&ty>=430&&ty<480)//音量-
		{
	       		
			system("echo volume -2 >> /pipe");
		}
		if(tx>=325&&tx<375&&ty>=430&&ty<480)//退出
	   {   
		   system("killall -kill mplayer");
	       break;	
	   }
		
       if(tx>=425&&tx<475&&ty>=430&&ty<480)//暂停/继续
		{
	       		system("echo pause >> /pipe");
			
		}			
	    if(tx>=525&&tx<575&&ty>=430&&ty<480)//音量+
		{
	       system("echo volume +2 >> /pipe");
			
		} 
	   
		if(tx>=625&&tx<=675&&ty>=430&&ty<480)//下一首
	   {   
		         if(i>3){
					 i=0;
				 }
	       		 system("killall -kill mplayer");
				 system(yinyue[i++]);
	   }
	 
	 }
	 return 0;
}
/*视频功能*/
int video(){
	 showbmp("/system_bmp/system_shipin/v_kuaitui.bmp",130,440,40,40);//快退
	 showbmp("/system_bmp/system_shipin/v_yinliang-.bmp",230,440,40,40);//音量-
	 showbmp("/system_bmp/system_shipin/v_tui.bmp",330,440,40,40);      //退出
	 showbmp("/system_bmp/system_shipin/v_zanting.bmp",430,440,40,40);//暂停
	 showbmp("/system_bmp/system_shipin/v_yinliang+.bmp",530,440,40,40);//音量+
	 showbmp("/system_bmp/system_shipin/v_kuaijin.bmp",630,440,40,40);//快进
	 
	 system("rm /pipe");
	 system("mkfifo /pipe");
	 system("chmod 777 pipe");
	 system("mplayer -slave -quiet -input  file=/pipe -geometry  0:0 -zoom -x 800 -y 440  /bin/dream.avi &");
	 while(1)
     {
		get_xy();
		 if(tx>=130&&tx<170&&ty>=440&&ty<=480)//快退
		{
	       		system("echo seek -5 >/pipe");
		}
		 if(tx>=230&&tx<270&&ty>=440&&ty<480)//音量-
		{
	       		
			system("echo volume -2 >> /pipe");
		}
		if(tx>=330&&tx<370&&ty>=440&&ty<480)//退出
	   {   
		   system("killall -kill mplayer");
	       break;	
	   }
		
       if(tx>=430&&tx<470&&ty>=440&&ty<480)//暂停/继续
		{
	       		system("echo pause >> /pipe");
			
		}			
	    if(tx>=530&&tx<570&&ty>=440&&ty<480)//音量+
		{
	       system("echo volume +2 >> /pipe");
			
		} 
	   
		if(tx>=630&&tx<=670&&ty>=440&&ty<480)//快进
	   {   
		   system("echo seek +5 >/pipe");	
	   }
	
		 
	 }
}
int camera_flag=0;
void *fun(void *arg)
{
		while(1)
	{
	// 实时监控
		/* 获取摄像头数据      存放jpg文件流*/
		linux_v4l2_get_fream(&freambuf);
		if(camera_flag==1)//暂停、继续
        {
			camera_flag=0;
			while(1)
			{
				if(camera_flag==2)
				{
					camera_flag=0;
					break;
				}
			}
			
		}	

		/* 显示摄像头图像*/
		lcd_draw_jpg(0, 0, NULL, freambuf.buf, freambuf.length, 0);
        if(camera_flag==3)//拍照
		{
			camera_flag=0;
		  //新建空白的jpg文件
           int jpg_fd=open("1.jpg",O_RDWR|O_CREAT);  
		  //把摄像头数据写入到jpg文件
		  write(jpg_fd,freambuf.buf,freambuf.length);
          //关闭jpg文件
           close(jpg_fd);
            sleep(1);		   
		}
        //回显
       if(camera_flag==4)
       {
		  while(1)
		  {
			camera_flag=0;
			lcd_draw_jpg(0, 0, "1.jpg", NULL,  NULL, 0);  
			if(camera_flag==2)
			{
				camera_flag=0;
				break;
			}				
		  }
		   
	   }		   
	}
}
int camera(){
		/* 打开LCD屏幕*/
	int lcd_fd=lcd_open();
	
	/* 初始化摄像头设备*/
	linux_v4l2_device_init("/dev/video7");
	
	/* 启动摄像头*/
	linux_v4l2_start_capturing();
	pthread_t pt2;
	int pid = pthread_create(&pt2,NULL,fun,NULL);
	  if(pid <0)
	  {
	    printf("creat pthread fail\n");
	  }
     printf("creat pthread ok\n");
	
	while(1)
	{   
  
		 get_xy();
		 if(tx>700&&ty<200)//暂停
		 {
			camera_flag=1; 
		 }
		 if(tx>700&&ty<200)//继续
		 {
			camera_flag=2; 
		 }
		 if(tx>700&&ty<200)//拍照
		 {
			camera_flag=3; 
		 }
		 if(tx>700&&ty<200)//回显
		 {
			camera_flag=4; 
		 }
		 
	}
	
	/* 停止摄像头*/
	linux_v4l2_stop_capturing();
	/* 卸载摄像头*/
	linux_v4l2_device_uinit("/dev/video7");

	lcd_close(lcd_fd);
	
}
//启动界面
 int start()
 {
    //显示启动背景
    showbmp("/system_bmp/qidong.bmp",0,0,800,480);
    //进度条
	int a=90;//进度条x轴的起始点
    while(1)
    {
		
	  showbmp("/system_bmp/jindu.bmp",a,300,40,40);
      a++;
      usleep(5000);
      if(a==600)
	  {
		  
		break;  
	  }		  
			
	}		
	 
 }
int main(int argc,char**argv)
{
     //启动界面
	 start();
	while(1)
	{
     //主界面
	 showbmp("/system_bmp/zhujiemian.bmp",0,0,800,480);
	 /*加载功能图标*/
	 showbmp("/system_bmp/xiangce.bmp",60,180,80,120);
	 showbmp("/system_bmp/yinyue.bmp",260,180,80,120);
	 showbmp("/system_bmp/shiping.bmp",460,180,80,120);
	 showbmp("/system_bmp/jiankong.bmp",660,180,80,120);
	 get_xy();
     if(tx>=60&&tx<140&&ty>=180&&ty<300)//相册按钮区域
     {
		photo();  
	 }	
	if(tx>=260&&tx<340&&ty>=180&&ty<300)//音乐按钮区域
     {
		music();  
	 }
	 if(tx>=460&&tx<540&&ty>=180&&ty<300)//视频按钮区域
     {
		video();  
	 }
	 if(tx>=660&&tx<740&&ty>=180&&ty<300)//监控按钮区域
     {
		camera();  
	 }		
	}
  
  
  return 0;   
}