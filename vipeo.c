#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "sample_comm.h"


VIDEO_NORM_E gs_enNorm = VIDEO_ENCODING_MODE_PAL;
HI_U32 gs_u32ViFrmRate = 0;


MPP_CHN_S Src;
MPP_CHN_S Dst;


HI_S32 VIPEO_16D1(HI_VOID)
{
	HI_U32 u32ViChnCnt = 4;

	MPP_SYS_CONF_S SysConf;			//SYS

	VI_DEV_ATTR_S ViDevAttr;//VI
	VI_CHN_ATTR_S ViChnAttr;
	VI_DEV ViDev[2] = {0,1};
	VI_CHN ViChn[4] = {0,1,2,3};
	VI_USERPIC_ATTR_S ViUserPic;
	VI_CHN_STAT_S ViStat;
	


	VB_CONF_S VbConf;	//VPSS
	//VPSS_GRP VpssGrp;
	VPSS_GRP_ATTR_S GrpAttr;
	VPSS_CHN_ATTR_S VpssChnAttr;
	VPSS_CHN_MODE_S VpssChnMode;
	HI_U32 Depth;
	//VPSS_GRP_PARAM_S GrpParam;
	VPSS_GRP VpssGrp[4] = {0,1,2,3};
	VPSS_CHN VpssChn = 3;


	VO_DEV VoDev;		//VO
	VO_CHN VoChn[4] = {0,1,2,3};
	VO_PUB_ATTR_S VoPubAttr;
	VO_VIDEO_LAYER_ATTR_S VoVideoLayerAttr;
	VO_CHN_ATTR_S VoChnAttr;

	HI_S32 i,row,line;
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32BlkSize;
	/**************************************/
	gs_u32ViFrmRate = (VIDEO_ENCODING_MODE_PAL == gs_enNorm)?25:30;

	memset(&VbConf, 0, sizeof(VB_CONF_S));
	memset(&ViDevAttr, 0, sizeof(VI_DEV_ATTR_S));
	memset(&ViChnAttr, 0, sizeof(VI_CHN_ATTR_S));
	memset(&ViUserPic, 0, sizeof(VI_USERPIC_ATTR_S));
	memset(&ViStat, 0, sizeof(VI_CHN_STAT_S));

	memset(&VpssChnMode, 0, sizeof(VPSS_CHN_MODE_S));
	//memset(&GrpParam, 0, sizeof(GrpParam));
	memset(&GrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));
	memset(&VpssChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));
	memset(&SysConf, 0, sizeof(MPP_SYS_CONF_S));
      

	SysConf.u32AlignWidth = 16;
        VbConf.u32MaxPoolCnt = 64;
	u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(VIDEO_ENCODING_MODE_PAL, PIC_D1, PIXEL_FORMAT_YUV_SEMIPLANAR_422, SysConf.u32AlignWidth);
	VbConf.astCommPool[0].u32BlkSize = u32BlkSize;
	VbConf.astCommPool[0].u32BlkCnt = u32ViChnCnt * 8;
	
	/*****************	SYS	********************/
	HI_MPI_SYS_Exit();
	HI_MPI_VB_Exit();
	s32Ret = HI_MPI_VB_SetConf(&VbConf);
	if(s32Ret != HI_SUCCESS)
	{
		printf("set sys conf alignwidth failed !\n");
		exit(-1);
	}
	s32Ret = HI_MPI_VB_Init();
	if(s32Ret != HI_SUCCESS)
	{
		printf("SYS VB init failed!\n");
		exit(-1);
	}
	s32Ret = HI_MPI_SYS_SetConf(&SysConf);
	if(s32Ret != HI_SUCCESS)
	{
		printf("sysy set conf failed 1\n");
		exit(-1);
	}
    	s32Ret = HI_MPI_SYS_Init();
	if(s32Ret != HI_SUCCESS)
	{
		printf("mpi sysy init failed !\n");
		exit(-1);
	}


	/******************	VI	*****************/
	for(i=0; i<1; i++)
	{
		HI_MPI_VI_DisableDev(ViDev[i]);
	}
	ViDevAttr.enIntfMode = VI_MODE_BT656;
	ViDevAttr.enWorkMode = VI_WORK_MODE_4Multiplex;
	ViDevAttr.enScanMode = VI_SCAN_INTERLACED;
	ViDevAttr.s32AdChnId[0] = -1;
	ViDevAttr.s32AdChnId[1] = -1;
	ViDevAttr.s32AdChnId[2] = -1;
	ViDevAttr.s32AdChnId[3] = -1;
	for(i=0; i<1; i++)
	{	
		switch(i)
		{
			case 0:
				ViDevAttr.au32CompMask[0] = 0xff000000;
        			ViDevAttr.au32CompMask[1] = 0x00;
				s32Ret = HI_MPI_VI_SetDevAttr(ViDev[0], &ViDevAttr);
				if(s32Ret != HI_SUCCESS)
				{
					printf("set device ID = %d attr failed!\n", i);
					exit(-1);
				}
				break;
			case 1:	
				ViDevAttr.au32CompMask[0] = 0x00ff0000;
                                ViDevAttr.au32CompMask[1] = 0x00;
                                s32Ret = HI_MPI_VI_SetDevAttr(ViDev[1], &ViDevAttr);
                                if(s32Ret != HI_SUCCESS)
				{
                                        printf("set device ID = %d attr failed!\n", i);
					exit(-1);
				}
                                break;
			default :
				break;
		}
	}
	for(i=0; i<1; i++)
	{
		s32Ret = HI_MPI_VI_EnableDev(ViDev[i]);
		if(s32Ret != HI_SUCCESS)
		{
			printf("enable %d dev failed!\n", i);
			exit(-1);
		}
	}
	ViChnAttr.stCapRect.s32X = 0;
	ViChnAttr.stCapRect.s32Y = 0;
	ViChnAttr.stCapRect.u32Width = 720;
	ViChnAttr.stCapRect.u32Height = 576;
	ViChnAttr.stDestSize.u32Width = 720;
	ViChnAttr.stDestSize.u32Height = 576;
	ViChnAttr.enCapSel = VI_CAPSEL_BOTH;
	ViChnAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
	ViChnAttr.bMirror = HI_FALSE;
	ViChnAttr.bFilp = HI_FALSE;
	ViChnAttr.bChromaResample = HI_FALSE;
	ViChnAttr.s32SrcFrameRate = -1;
	ViChnAttr.s32FrameRate = -1;
	for(i=0; i <4; i++)
	{
		s32Ret = HI_MPI_VI_SetChnAttr(ViChn[i],&ViChnAttr);
		if(s32Ret != HI_SUCCESS)
		{
			printf("set vi attr chn %d failed!\n",i);
			exit(-1);
		}
	}
	for(i=0; i<4; i++)
	{
		s32Ret = HI_MPI_VI_EnableChn(ViChn[i]);
		if(s32Ret != HI_SUCCESS)
		{
			printf("enable vi chn %d failed!\n",i);
			exit(-1);
		}
	}
	ViUserPic.bPub = HI_TRUE;
	ViUserPic.enUsrPicMode = VI_USERPIC_MODE_BGC;
	for(i=0; i<4; i++)
	{
		ViUserPic.unUsrPic.stUsrPicBg.u32BgColor = 0x00778899;
		s32Ret = HI_MPI_VI_SetUserPic(ViChn[i], &ViUserPic);
		if(s32Ret != HI_SUCCESS)
		{
			printf("set user pic failed ! %d %x",i,s32Ret);
			exit(-1);
		}
		s32Ret = HI_MPI_VI_EnableUserPic(ViChn[i]);
                if(s32Ret != HI_SUCCESS)
                {
                        printf("enable user pic failed ! %d %x",i,s32Ret);
                        exit(-1);
                }

	}

	/******************		VPSS		**************************/
	GrpAttr.u32MaxW = 1920;
	GrpAttr.u32MaxH = 1080;
	GrpAttr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
	GrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	GrpAttr.bDrEn = HI_FALSE;
	GrpAttr.bDbEn = HI_FALSE;
	GrpAttr.bIeEn = HI_FALSE;
	GrpAttr.bNrEn = HI_FALSE;
	GrpAttr.bHistEn = HI_FALSE;
	for(i=0; i<4; i++)
	{
		s32Ret = HI_MPI_VPSS_CreatGrp(VpssGrp[i] , &GrpAttr);
		if(s32Ret != HI_SUCCESS)
		{
			printf("create vpss group %d failed!\n",i);
			exit(-1);
		}
	}
	VpssChnAttr.bSpEn = HI_FALSE;
	VpssChnAttr.bFrameEn = HI_TRUE;
	VpssChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_LEFT] = 2;
        VpssChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_RIGHT] = 2;
        VpssChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_TOP] = 2;
        VpssChnAttr.stFrame.u32Width[VPSS_FRAME_WORK_BOTTOM] = 2;
	VpssChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_LEFT] = 0x006495ed;
        VpssChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_RIGHT] = 0x006495ed;
        VpssChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_TOP] = 0x006495ed;
        VpssChnAttr.stFrame.u32Color[VPSS_FRAME_WORK_BOTTOM] = 0x006495ed;
	Depth = 7;
	
	VpssChnMode.enChnMode = VPSS_CHN_MODE_AUTO;
	VpssChnMode.u32Width = 1280/2;
	VpssChnMode.u32Height = 720/2;
	VpssChnMode.bDouble = HI_FALSE;
	VpssChnMode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;	

	for(i=0; i<4;i++)
	{
		s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp[i], VpssChn, &VpssChnAttr);
		if(s32Ret != HI_SUCCESS)
		{
			printf("set vpss chn %d failed ! code is %x\n", i, s32Ret);
			exit(-1);
		}
		s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp[i], VpssChn);
		if(s32Ret != HI_SUCCESS)	
		{
			printf("enable vpss chn %d failed!\n",i);
			exit(-1);
		}
		HI_MPI_VPSS_GetDepth(VpssGrp[i], VpssChn, &Depth);
        	if(Depth == 0) 
                	Depth = 7;
        	HI_MPI_VPSS_SetDepth(VpssGrp[i], VpssChn, &Depth);
		s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp[i], VpssChn, &VpssChnMode);
		if(s32Ret != HI_SUCCESS)
		{
			printf("set vpss chn mode %d failed !\n", i);
			exit(-1);
		}
		
	}
	for(i=0;i<4;i++)
	{
             s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp[i]);
                if(s32Ret != HI_SUCCESS)
                {
                        printf("start vpss %d failed!\n", i);
                        exit(-1);
                }
	}
	for(i=0; i<4;i++)
	{
	       	Src.enModId = HI_ID_VIU;
		Src.s32DevId = ViDev[i/4];
		Src.s32ChnId = ViChn[i];

        	Dst.enModId = HI_ID_VPSS;
		Dst.s32DevId = VpssGrp[i];
		Dst.s32ChnId = 0;
		s32Ret = HI_MPI_SYS_Bind(&Src ,&Dst);
		if(s32Ret != HI_SUCCESS)
		{
			printf("videv:id=%d chn:id=%d bind vpss:id=%d chn:id=%d failed!",ViDev[i/4],ViChn[i],VpssGrp[i],VpssChn);
			exit(-1);
		}
	}


	/******************	VPSS	*******************/
	/*
 	s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, PIC_D1, &stSize);
	if(s32Ret != HI_SUCCESS)
	{
		printf("SAMPLE_COMM_SYS_GetPicSize failed!\n");
		goto END_16D1_1;
	}
	GrpAttr.u32MaxW = 720;
	GrpAttr.u32MaxH = (VIDEO_ENCODING_MODE_PAL==gs_enNorm)?576:480;
	GrpAttr.bDrEn = HI_FALSE;
	GrpAttr.bDbEn = HI_FALSE;
	GrpAttr.bIeEn = HI_TRUE;
	GrpAttr.bNrEn = HI_TRUE;
	GrpAttr.bHistEn = HI_TRUE;
	GrpAttr.enDieMode = VPSS_DIE_MODE_AUTO;
	GrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;

	s32Ret = SAMPLE_COMM_VPSS_Start(s32VpssGrpCnt, &stSize, VPSS_MAX_CHN_NUM, NULL);
	if(s32Ret != HI_SUCCESS)
	{
		printf("start vpss failed !\n");
		goto END_16D1_1;
	}

	s32Ret = SAMPLE_COMM_VI_BindVpss(enViMode);
	if(s32Ret != HI_SUCCESS)
	{
		printf("vi bind vpss failed !\n");
		goto END_16D1_2;
	}*/
	/*********************		VO	***********************************/	
	VoDev = SAMPLE_VO_DEV_DHD0;
	VoPubAttr.u32BgColor = 0x00708090;
	VoPubAttr.enIntfType = VO_INTF_VGA;
	VoPubAttr.enIntfSync = VO_OUTPUT_720P60;
	//VoPubAttr.stSyncInfo = ;
	VoPubAttr.bDoubleFrame = HI_FALSE;
	
	s32Ret = HI_MPI_VO_SetPubAttr(VoDev, &VoPubAttr);
	if(s32Ret != HI_SUCCESS)
	{
		printf("set vo pub attr failed! %x\n",s32Ret);
		exit(-1);
	}
	s32Ret = HI_MPI_VO_Enable(VoDev);
	if(s32Ret != HI_SUCCESS)
	{
		printf("enable vo failed !%x \n",s32Ret);
		exit(-1);
	}

	s32Ret = HI_MPI_VO_GetVideoLayerAttr(VoDev, &VoVideoLayerAttr);
	if(s32Ret != HI_SUCCESS)
	{
		printf("get video layer attr failed %x !\n",s32Ret);
		exit(-1);
	}
	VoVideoLayerAttr.stDispRect.s32X = 0;
    	VoVideoLayerAttr.stDispRect.s32Y = 0;
    	VoVideoLayerAttr.stDispRect.u32Width = 1280;
    	VoVideoLayerAttr.stDispRect.u32Height = 720;
        VoVideoLayerAttr.stImageSize.u32Width = 1280;
    	VoVideoLayerAttr.stImageSize.u32Height = 720;
	VoVideoLayerAttr.u32DispFrmRt = 60;
	VoVideoLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;

	s32Ret = HI_MPI_VO_SetVideoLayerAttr(VoDev, &VoVideoLayerAttr);
	if(s32Ret != HI_SUCCESS)
	{
		printf("set video layer attr failed !\n");
		exit(-1);
	}
	s32Ret = HI_MPI_VO_EnableVideoLayer(VoDev);
	if(s32Ret != HI_SUCCESS)
	{
		printf("enable vo failed!\n");
		exit(-1);
	}
	for(row=0; row<2; row++)
	{
		for(line=0;line<2;line++)
		{
			VoChnAttr.u32Priority = 0;
			VoChnAttr.stRect.s32X = (1280/2)*line;
      			VoChnAttr.stRect.s32Y = (720/2)*row;
      			VoChnAttr.stRect.u32Width = 1280/2;	
      			VoChnAttr.stRect.u32Height = 720/2;
			VoChnAttr.bDeflicker = HI_FALSE;
			s32Ret = HI_MPI_VO_SetChnAttr(VoDev, VoChn[line+2*row], &VoChnAttr);
			if(s32Ret != HI_SUCCESS)
			{
				printf("vochn %d failed!\n",VoChn[line+2*row]);
				exit(-1);
				
			}
		
	    		s32Ret = HI_MPI_VO_EnableChn(VoDev, VoChn[line + 2*row]);
                	if(s32Ret != HI_SUCCESS)
                	{
                        	printf("enable vo chn failed !\n",VoChn[i]);
                        	exit(-1);
                	}
		}
	}
	printf("set chn attr end,enable chn end!\n");

	for(i=0; i<4; i++)
	{
		Src.enModId = HI_ID_VPSS;	
		Src.s32DevId = VpssGrp[i];
		Src.s32ChnId = VpssChn;

		Dst.enModId = HI_ID_VOU;
		Dst.s32DevId =	VoDev;
		Dst.s32ChnId = VoChn[i];
		s32Ret = HI_MPI_SYS_Bind(&Src, &Dst);
		if(s32Ret != HI_SUCCESS)
		{
			printf("vpss bind vo chn  %d failed!\n",i);
			exit(-1);
		}
	}
	for(i=0;i<4;i++)
	{
		HI_MPI_VO_ChnShow(VoDev, VoChn[i]);
	}

	s32Ret = HI_MPI_VO_SetAttrBegin(VoDev);
	if(s32Ret != HI_SUCCESS)
	{
		printf("set vo attr strat failed !%x\n",s32Ret);
		exit(-1);
	}
	s32Ret = HI_MPI_VO_SetAttrEnd(VoDev);
	if(s32Ret != HI_SUCCESS)
	{
                printf("set vo attr strat failed !%x\n",s32Ret);
                exit(-1);
        }

		
	printf("can you see the screen?\n");	
 	s32Ret = HI_MPI_VI_Query(ViChn[0],&ViStat);
        if(s32Ret != HI_SUCCESS)
        {
		printf("query chn %d failed!%x\n",i,s32Ret);
        }else{
                printf("chn %d stat:::: enable:%d;interrupt count:%d;frame rate:%d;lost interrupt:%d;vbfailed:%d;picw:%d;pich:%d",i,ViStat.bEnable,\
                                        ViStat.u32IntCnt,ViStat.u32FrmRate,ViStat.u32LostInt,ViStat.u32VbFail,ViStat.u32PicWidth,ViStat.u32PicHeight);
        }

	while(1)
	{
		for(i=0; i<4;i++)
		{
			s32Ret = HI_MPI_VI_Query(i,&ViStat);
			if(s32Ret != HI_SUCCESS)
			{
				printf("query chn %d failed!%x\n",i,s32Ret);
			}else{
				printf("chn %d stat:::: enable:%d;interrupt count:%d;frame rate:%d;lost interrupt:%d;vbfailed:%d;picw:%d;pich:%d",i,ViStat.bEnable,\
					ViStat.u32IntCnt,ViStat.u32FrmRate,ViStat.u32LostInt,ViStat.u32VbFail,ViStat.u32PicWidth,ViStat.u32PicHeight);
			}
			sleep(2);
	
		}
	}


	/******************	VO	**********************/
	/*VoDev = SAMPLE_VO_DEV_DHD0;
	u32WndNum = 16;
	enVoMode = VO_MODE_16MUX;
	if(VIDEO_ENCODING_MODE_PAL == gs_enNorm)
	{
		VoPubAttr.enIntfSync = VO_OUTPUT_1080P50;
	}else{
		VoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
	}
	VoPubAttr.enIntfType = VO_INTF_VGA;
	VoPubAttr.u32BgColor = 0x000000ff;
	VoPubAttr.bDoubleFrame = HI_TRUE;

	s32Ret = SAMPLE_COMM_VO_StartDevLayer(VoDev, &VoPubAttr, gs_u32ViFrmRate);
	if(s32Ret != HI_SUCCESS)
	{
		printf("start SAMPLE_COMM_VO_StartDevLayer failed!\n");
		goto END_16D1_3;
	}

	s32Ret = SAMPLE_COMM_VO_StartChn(VoDev, &VoPubAttr, enVoMode);
	if(s32Ret !=  HI_SUCCESS)
	{
		printf("start SAMPLE_COMM_VO_StartChn failed!\n");
		goto END_16D1_4
	}

	for(i=0; i<u32WndNum; i++)
	{
		VoChn = i;
		VpssGrp = i;
		
		s32Ret = SAMPLE_COMM_VO_BindVpss(VoDev, VoChn, VpssGrp, VpssChn_VoHD0);
		if(s32Ret != HI_SUCCESS)
		{
			printf("start vo failed !\n");
			goto END_16D1_4
		}
	}
	*/
	/*****************	exit	*************************************/


END_16D1_4:
	for(i=0; i<16; i++)
	{
	 	Src.enModId = HI_ID_VPSS;
                Src.s32DevId = VpssGrp[i];
                Src.s32ChnId = VpssChn;
                Dst.enModId = HI_ID_VOU;
                Dst.s32DevId =  VoDev;
                Dst.s32ChnId = VoChn[i];
                s32Ret = HI_MPI_SYS_UnBind(&Src, &Dst);
                if(s32Ret != HI_SUCCESS)
                {
                        printf("vpss bind vo chn  %d failed!\n",i);
                        exit(-1);
                }

	}	

	for(i=0; i<16; i++)
	{
		s32Ret = HI_MPI_VO_DisableChn(VoDev, i);
        	if (s32Ret != HI_SUCCESS)
        	{
        	    	SAMPLE_PRT("failed with %#x!\n", s32Ret);
            		return HI_FAILURE;
        	}
	}
	s32Ret = HI_MPI_VO_DisableVideoLayer(VoDev);
    	if (s32Ret != HI_SUCCESS)
    	{
        	SAMPLE_PRT("failed with %#x!\n", s32Ret);
        	return HI_FAILURE;
    	}

    	s32Ret = HI_MPI_VO_Disable(VoDev);
    	if (s32Ret != HI_SUCCESS)
    	{
        	SAMPLE_PRT("failed with %#x!\n", s32Ret);
        	return HI_FAILURE;
    	}

	/*
	VoDev = SAMPLE_COMM_DEV_DHD0;
	u32WndNum = 16;
	enVoMode = VO_MODE_16MUX;
	for(i=0; i<u32WndNum; i++)
	{
		VoChn = i;
		VpssGrp = i;
		SAMPLE_COMM_VO_UnBindVpss(VoDev, VoChn, VpssGrp, VpssChn_VoHD0);
	}
	SAMPLE_COMM_VO_StopChn(VoDev, enVoMode);
	SAMPLE_COMM_VO_Stop_DevLayer(VoDev);
	*/
END_16D1_3:
	for(i=0; i<16;i++)
        {
                Src.enModId = HI_ID_VIU;
                Src.s32DevId = ViDev[i/4];
                Src.s32ChnId = ViChn[i];

                Dst.enModId = HI_ID_VPSS;
                Dst.s32DevId = VpssGrp[i];
                Dst.s32ChnId = VpssChn;
                s32Ret = HI_MPI_SYS_UnBind(&Src ,&Dst);
                if(s32Ret != HI_SUCCESS)
                {
                        printf("videv:id=%d chn:id=%d bind vpss:id=%d chn:id=%d failed!",ViDev[i/4],ViChn[i],VpssGrp[i],VpssChn);
                        exit(-1);
                }
        }
	//SAMPLE_COMM_VI_UnBindVpss(enViMode);
END_16D1_2:
	for(i=0; i<16; i++)
	{
		s32Ret = HI_MPI_VPSS_StopGrp(VpssGrp[i]);
		if(s32Ret != HI_SUCCESS)
		{
			printf("stop vpss %d failed!",VpssGrp);
			exit(-1);
		}
		s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp[i], VpssChn);
		if(s32Ret != HI_SUCCESS)
                {
                        printf("disable vpss  chn %d failed!",VpssGrp);
                        exit(-1);
                }
		s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp[i]);
		if(s32Ret != HI_SUCCESS)
                {
                        printf("destory vpss %d failed!",VpssGrp);
                        exit(-1);
                }
	}
	//SAMPLE_COMM_VPSS_Stop(s32VpssGrpCnt, VPSS_MAX_CHN_NUM);
END_16D1_1:
	for(i=0; i<16; i++)
	{
		s32Ret = HI_MPI_VI_DisableChn(ViChn[i]);
		if (HI_SUCCESS != s32Ret)
        	{
            		SAMPLE_PRT("SAMPLE_COMM_VI_StopChn failed with %#x\n",s32Ret);
            		return HI_FAILURE;
        	}
	}
	for(i=0; i<4; i++)
	{
		s32Ret = HI_MPI_VI_DisableDev(ViDev[i]);
		if (HI_SUCCESS != s32Ret)
        	{
            		SAMPLE_PRT("SAMPLE_COMM_VI_StopDev failed with %#x\n", s32Ret);
            		return HI_FAILURE;
        	}
	}
	//SAMPLE_COMM_VI_Stop(enViMode);
END_16D1_0:
	HI_MPI_SYS_Exit();
	HI_MPI_VB_Exit();
	//SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}
/*
 *
 *
 */
/*void VencThread()
{
	HI_S32 s32Ret,i;

	VENC_GRP VencGrp[4]={0,1,2,3};
	VENC_CHN VencChn[4]={0,1,2,3}
	VENC_CHN_ATTR_S VencChnAttr;

	memset(&VencChnAttr, 0, sizeof(VENC_CHN_ATTR_S));

	for(i=0; i<4;i++)
	{
		s32Ret = HI_MPI_VENC_CreatGroup();
		if(s32Ret != HI_SUCCESS)
		{
			printf("create venc group %d failed ! %x",i, s32Ret);
			exit(-1);
		}
	}
	VencChnAttr.stVenAttr.enType = PT_H264;
        VencChnAttr.stVenAttr.stAttrH264e.u32MaxPicWidth  = 1280;
        VencChnAttr.stVenAttr.stAttrH264e.u32MaxPicHeight  = 720;
        VencChnAttr.stVenAttr.stAttrH264e.u32BufSize  = 1280*720*1.5;
        VencChnAttr.stVenAttr.stAttrH264e.u32Profile  = 0;
        VencChnAttr.stVenAttr.stAttrH264e.bByFrame  = HI_FALSE; //true:frame || false:package
        VencChnAttr.stVenAttr.stAttrH264e.bField  = HI_FALSE;//only support frame encode
        VencChnAttr.stVenAttr.stAttrH264e.bMainFrame  = HI_TRUE; //only support main 
        VencChnAttr.stVenAttr.stAttrH264e.u32Priority  = 0;
        VencChnAttr.stVenAttr.stAttrH264e.bVIField  = HI_FALSE;//only support frame mode
        VencChnAttr.stVenAttr.stAttrH264e.u32PicWidth  = 720;
	VencChnAttr.stVenAttr.stAttrH264e.u32PicHeight = 576;

	VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
        VencChnAttr.stRcAttr.stAttrH264Cbr.u32Gop = 30;
        VencChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime = 1;
        VencChnAttr.stRcAttr.stAttrH264Cbr.u32ViFrmRate = ;
        VencChnAttr.stRcAttr.stAttrH264Cbr.Fr32TargetFrmRate = ;
        VencChnAttr.stRcAttr.stATtrH264Cbr.u32BitRate = ;
        VencChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 0;
	//VencChnAttr.stRcAttr.pRcAttr = ;
	for(i=0; i<4;i++)
	{
		s32Ret = HI_MPI_VENC_CreateChn(VencChn[i],&VencChnAttr);
		if(s32Ret != HI_SUCCESS)
		{
			printf("create venc chn %d falied !%x",i, s32Ret);
			exit(-1);
		}
	}
	for(i=0; i<4;i++)
        {
                s32Ret = HI_MPI_VENC_RegisterChn(VencGrp[i],VencChn[i]);
                if(s32Ret != HI_SUCCESS)
                {
                        printf("create venc chn %d falied !%x",i, s32Ret);
                        exit(-1);
                }
        }



}


AUDIO_DEV AiDev[2] = {0,1};
AUDIO_DEV AioDev = 2;
AUDIO_DEV AoDev = 3;

AIO_ATTR_S AiAttr;
AIO_




void audiothread()
{

	HI_S32 s32Ret = 0;
	AiAttr.enSamplerate = ;
	AiAttr.enBitwidth = ;
	AiAttr.enWorkmode = ;
	AiAttr.enSoundmode = ;
	AiAttr.u32EXFlag = ;
	AiAttr.u32FrmNum = ;
	AiAttr.u32PtNumPerFrm = ;
	AiAttr.u32ChnCnt = ;
	AiAttr.u32ClkSel = ;

	s32Ret = HI_MPI_AI_SetPubAttr(AiDev[0], &AiAttr);
	if(s32Ret != HI_SUCCESS)
	{
		printf("");
		
	}
	





}




*/






/*function : VIPEO_HandleSig
*
*author:erbort
*/
void VIPEO_HandleSig(HI_S32 sig)
{
	if(sig == SIGINT || sig == SIGTSTP)
	{
		HI_MPI_SYS_Exit();
		HI_MPI_VB_Exit();
		printf("\033[0;31m program termination abnormally! \033[0;39m\n");
	}
	exit(-1);
}


/*function : main
*
* author:erbort
 */
int main(int argc , char *argv[])
{
	HI_S32 s32Ret;
	signal(SIGINT, VIPEO_HandleSig);
	signal(SIGTERM, VIPEO_HandleSig);
	
	s32Ret = VIPEO_16D1();
	if(s32Ret == HI_SUCCESS)
		printf("program exit normally!\n");
	else
		printf("program exit abnormally!\n");
	exit(s32Ret);
}


#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif

