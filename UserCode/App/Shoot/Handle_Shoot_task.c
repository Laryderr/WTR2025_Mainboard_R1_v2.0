/**
 * @file Shoot_task.c
 * @author Lary (you@domain.com)
 * @brief  WTR2025R1 后抛投篮线程
 * @version 0.1
 * @date 2025-03-10
 * @note 投篮线程需要接受
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "Handle_Shoot_task.h"

 Shoot_Task_T my_Shoot_Task_T;
 /****************************************************线程定义******************************************/
osThreadId_t unitree_shoot_ctrl_TaskHandle;
const osThreadAttr_t unitree_shoot_ctrl_Task_attributes = {
    .name       = "unitree_shoot_ctrl_Task",
    .stack_size = 128*8,
    .priority   = (osPriority_t)osPriorityNormal,
};
void Handle_Shoot_Task(void *argument);


/**
 * @brief Unitree电机后抛手动投篮控制线程创建
 * 
 */
void Handle_Shoot_TaskStart(void)
{
    unitree_shoot_ctrl_TaskHandle = osThreadNew(Handle_Shoot_Task,NULL,&unitree_shoot_ctrl_Task_attributes);
}

/**************************************************内部控制函数***********************************************************************/

/**
 * @brief 角编码器位置pid初始化
 * 
 * @param upid 
 * @param ref 
 * @param KP 
 * @param KI 
 * @param KD 
 */
void Encoder_Speedpid_Init(PID_t *upid,float ref, float KP, float KI, float KD)
{
    upid->ref         = ref;   /* 设定目标值 */
    upid->output      = 0.0; /* 期望输出值 */
    upid->integral    = 0.0; /* 积分值 */
    upid->cur_error   = 0.0; /* Error[1] */
    upid->error[0]    = 0.0; /* Error[-1] */
    upid->error[1]    = 0.0; /* Error[-2] */
    upid->KP          = KP;  /* 比例常数 Proportional Const */
    upid->KI          = KI;  /* 积分常数 Integral Const */
    upid->KD          = KD;  /* 微分常数 Derivative Const */
    upid->outputMax   = 0.3;
    upid->outputMin   = -0.3;
}

/**
 * @brief 角编码器位置伺服
 * 
 * @param upid 
 * @param Feedback_value 
 */
float Encoder_SpeedServo(PID_t *upid, float ref, float Feedback_value)
{
    upid->ref = ref;
    upid->fdb = Feedback_value;
    upid->cur_error = (float)(upid->ref - upid->fdb); /* 计算偏差 */

    //upid->integral += upid->cur_error;
    upid->output        = (upid->KP * upid->cur_error)                        /* 比例环节 */
                        + (upid->KI * upid->error[0])                     /* 积分环节 */
                        + (upid->KD * (upid->cur_error - upid->error[0])); /* 微分环节 */
    upid->error[0] = upid->cur_error;
    
    if(upid->output >= upid->outputMax) upid->output = upid->outputMax;
    if(upid->output <= upid->outputMin) upid->output = upid->outputMin;

    return (upid->output); /* 返回计算后输出的数值 */
}

void Back_to_init()
{
    unitree_DunkMotor_t[3].cmd.Pos = 0;
    unitree_DunkMotor_t[3].cmd.K_P = 0;
    unitree_DunkMotor_t[3].cmd.K_W = 0.1;
    unitree_DunkMotor_t[3].cmd.W = 1.5;
    unitree_DunkMotor_t[3].cmd.T = 0;
}

/**
 * @brief 寻找投篮电机姿态初始位置
 * @return float 
 */
HAL_StatusTypeDef Shootmotor_Contect_InitialPos(Shoot_Task_T * my_T)
{
    unitree_DunkMotor_t[3].cmd.Pos = 0;
    unitree_DunkMotor_t[3].cmd.K_P = 0;
    unitree_DunkMotor_t[3].cmd.K_W = 0;
    unitree_DunkMotor_t[3].cmd.W = 0;
    unitree_DunkMotor_t[3].cmd.T = 0;

    unitree_DunkMotor_t[5].cmd.Pos = 0;
    unitree_DunkMotor_t[5].cmd.K_P = 0;
    unitree_DunkMotor_t[5].cmd.K_W = 0;
    unitree_DunkMotor_t[5].cmd.W = 0;
    unitree_DunkMotor_t[5].cmd.T = 0;   
    //osDelay(INITIAL_DELAY_TIME);//使投篮臂自然垂下
    
    for (uint8_t i = 0; i < 10; i++)
    {
        my_T->Shootball_InitialPos[0] += unitree_DunkMotor_t[3].data.Pos;
        my_T->Shootball_InitialPos[1] += unitree_DunkMotor_t[5].data.Pos;
    }
    my_T->Shootball_InitialPos[0] = my_T->Shootball_InitialPos[0]/10.0f;
    my_T->Shootball_InitialPos[1] = my_T->Shootball_InitialPos[1]/10.0f;
    
    //my_T->Shootball_InitialPos[0] =my_T->Shootball_InitialPos[0] - (Encoder_VertPos - encoderData.angle)/DegreetoRad*UNITREE_REDUCTION_RATE ;
    //my_T->Shootball_InitialPos[1] = (Encoder_VertPos - encoderData.angle)/DegreetoRad*UNITREE_REDUCTION_RATE + my_T->Shootball_InitialPos[1];
    
    for (uint8_t i = 0; i < 2; i++)
    {
        if(my_T->Shootball_InitialPos[i] != 0)
        {
            my_T->Foundshoot_InitialPos_Flag[i] = 1;
        }else return HAL_ERROR; 
    }
    return HAL_OK;
    
}

/**
 * @brief 投篮电机初始化
 * 
 * @param my_T 
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef Shootmotor_Init(Shoot_Task_T * my_T)
{
    my_T->myshoot_status = SHOOT_IDLE;
    my_T->Foundshoot_InitialPos_Flag[0]=0;
    my_T->Foundshoot_InitialPos_Flag[1]=0;
    my_T->Shootball_InitialPos[0]=0;
    my_T->Shootball_InitialPos[1]=0;
    my_T->target_throw_deg = 0;
    my_T->target_throw_w   = 0;
    my_T->Shoot_Completed_Flag =0;
    my_T->shoot_point = 0;
    my_T->last_shoot_point = 0 ;
    my_T->camera_aim_flag = 0;
    my_T->camera_get_angle_flag = 0;
    my_T->shoot_count = 0;
    my_T->on_shoot_point = 0;

    my_T->Auto_shoot_point[0].actual_xpos = 0.861;
    my_T->Auto_shoot_point[0].actual_ypos = 0.5755;

    if(encoderData.angle == 0)
    {
        // return HAL_ERROR;
    }

    if (Shootmotor_Contect_InitialPos(my_T) == HAL_OK )
    {
        // return HAL_OK;
    }/*else return HAL_ERROR;*/
    
}

/**
 * @brief 投篮电机角速度解算
 * 
 * @param L 
 * @param H 
 * @return float 
 */
float Shot_Speed_Calculation(float L,float H,float throw_degree)
{
    
}


void Ball_Hold(float W)
{
    unitree_DunkMotor_t[5].cmd.Pos = my_Shoot_Task_T.Shootball_InitialPos[1]  + 1;
    unitree_DunkMotor_t[5].cmd.K_P = 0.4;
    unitree_DunkMotor_t[5].cmd.K_W = 0.15;
    unitree_DunkMotor_t[5].cmd.W = W;
    unitree_DunkMotor_t[5].cmd.T = 0;
    Unitree_motor_0Torque(3);
}


/****************************************************************************************************************************** 
 * @brief Unitree电机后抛投篮控制线程实现
 * 
 */
void Handle_Shoot_Task(void*argument)
{
    Shootmotor_Init(&my_Shoot_Task_T);
    my_Auto_Shoot_Task_T.now_train_distance = 0;
    my_Auto_Shoot_Task_T.train_step = 0.1;
    my_Auto_Shoot_Task_T.min_KW = 0.9086;
    my_Auto_Shoot_Task_T.max_KW = 0.0f;

    my_Shoot_Task_T.camera_yaw_turning = 0;
    for (;;)
    {
        // my_Auto_Shoot_Task_T.now_train_distance = MIN_TRAIN_DISTANCE + MyRemote_Data.btn_LeftCrossRight_press_count*my_Auto_Shoot_Task_T.train_step - MyRemote_Data.btn_LeftCrossLeft_press_count*my_Auto_Shoot_Task_T.train_step;
        // my_Auto_Shoot_Task_T.train_point[0] = BASKET_X - sin(PI/4)*my_Auto_Shoot_Task_T.now_train_distance;
        // my_Auto_Shoot_Task_T.train_point[1] = BASKET_Y + sin(PI/4)*my_Auto_Shoot_Task_T.now_train_distance;
        // my_Alldir_Chassis_t.test_KW = my_Auto_Shoot_Task_T.min_KW + MyRemote_Data.KW01 + MyRemote_Data.KW001 +
        //                            MyRemote_Data.KW0001 + MyRemote_Data.KW00001 ;
        osDelay(2);
        //遥控器控制投篮状态机
        if(BtnScan_Press(MyRemote_Data.btn_KnobR)||BtnScan_Press(MyRemote_Data.btn_RightCrossMid))
        {
            my_Shoot_Task_T.myshoot_status = SHOOT_ING;
        }
        if(BtnScan_Press(MyRemote_Data.btn_KnobL))
        {
            if(my_Shoot_Task_T.shoot_point != 7&&my_Shoot_Task_T.shoot_point != 1)
            {
                my_Shoot_Task_T.Shoot_Completed_Flag = 0;
                myHandle_State = HANDLE_IDLE_MODE;
            }
            my_Shoot_Task_T.myshoot_status = SHOOT_IDLE;
        }
        if(BtnScan_Press(MyRemote_Data.btn_RightCrossDown))
        {
            my_Shoot_Task_T.myshoot_status = SHOOT_BACK_TO_INIT;   
        }

/******************************************************挑战赛点位控制******************************************************************************/
        if (BtnScan_Press(MyRemote_Data.btn_RightCrossMid))
        {
            my_Shoot_Task_T.shoot_point = 8;
        }else if (BtnScan_Press(MyRemote_Data.btn_Btn1))
        {
            my_Shoot_Task_T.shoot_point = 7;
            my_Shoot_Task_T.on_shoot_point = 0;
        }else if(BtnScan_Press(MyRemote_Data.btn_JoystickR))
        {
            my_Shoot_Task_T.on_shoot_point = 0;
            my_Shoot_Task_T.shoot_point = 7;
        }else if(BtnScan_Press(MyRemote_Data.btn_RightCrossUp))
        {
            my_Shoot_Task_T.on_shoot_point = 0;
            my_Shoot_Task_T.shoot_point = 1;
        }

        my_Shoot_Task_T.dx = my_Alldir_Chassis_t.current_pos.xpos - my_Shoot_Task_T.Auto_shoot_point[my_Shoot_Task_T.shoot_point].X_offset;
        my_Shoot_Task_T.dy = my_Alldir_Chassis_t.current_pos.ypos - my_Shoot_Task_T.Auto_shoot_point[my_Shoot_Task_T.shoot_point].Y_offset;

        
        if(my_Shoot_Task_T.shoot_point == 7)
        {
            if(my_Shoot_Task_T.dx<=0.01&&my_Shoot_Task_T.dx>=-0.01&&my_Shoot_Task_T.dy<=0.01&&my_Shoot_Task_T.dy>=-0.01)
            {
                //覆盖投篮点位
                my_Shoot_Task_T.on_shoot_point = 1;
            }

            if(my_Shoot_Task_T.on_shoot_point == 0) 
            {
                //未覆盖投篮点位,但是已接近点位
                chassis_XYPoseServo_calc(my_Shoot_Task_T.Auto_shoot_point[my_Shoot_Task_T.shoot_point].X_offset,
                                    my_Shoot_Task_T.Auto_shoot_point[my_Shoot_Task_T.shoot_point].Y_offset);
                my_Shoot_Task_T.last_shoot_point = my_Shoot_Task_T.shoot_point;
                my_Alldir_Chassis_t.chassis_Aim_at_Basket(my_Shoot_Task_T.camera_yaw_turning);
            }else {
                //停止位置伺服，精瞄
                my_Alldir_Chassis_t.chassis_Aim_at_Basket(my_Shoot_Task_T.camera_yaw_turning);
                my_Alldir_Chassis_t.chassis_xpos_pid.output = 0;
                my_Alldir_Chassis_t.chassis_ypos_pid.output = 0;
                my_Alldir_Chassis_t.target_v.vx = 0;
                my_Alldir_Chassis_t.target_v.vy = 0;
            }
        }else if(my_Shoot_Task_T.shoot_point == 1)
        {
            if(MyRemote_Data.right_switch == 1)
            {
                my_Alldir_Chassis_t.chassis_Aim_at_Basket(my_Shoot_Task_T.camera_yaw_turning);
                my_Alldir_Chassis_t.target_v.vy = ((float)MyRemote_Data.usr_left_x)/283.3f*6.0f ;
                my_Alldir_Chassis_t.target_v.vx = ((float)MyRemote_Data.usr_left_y)/283.3f * (-1.0f)*6.0f;
            }else {
                my_Alldir_Chassis_t.target_v.vy = ((float)MyRemote_Data.usr_left_x)/283.3f*6.0f ;
                my_Alldir_Chassis_t.target_v.vx = ((float)MyRemote_Data.usr_left_y)/283.3f * (-1.0f)*6.0f;
                my_Alldir_Chassis_t.target_v.vw = ((float)MyRemote_Data.usr_right_x)/230.0f*2.0f;
            }
        }else my_Alldir_Chassis_t.YAWPosServo(my_Alldir_Chassis_t.current_pos.yaw_offset - 90);

/************************************************************************************************************************************/
        
        //升降机构固定在中部
        /*Expansion_Up.contract();
        Expansion_Down.contract();*/
        
        //不用
        // //计算篮球出手点距离篮筐中心水平距离
        // my_Shoot_Task_T.HorizontalDist_to_Basket = my_Alldir_Chassis_t.chassis_to_basket + 
        //     Motor_to_Chassis_L + cos(encoderData.angle-Encoder_VertPos-90+Deg_Ball_to_Arm)*Rotation_r_Of_Ball;
        // //计算篮球中心出手离地高度
        // my_Shoot_Task_T.ShotTime_Ball_Heigh = Motor_HeightMedium + sin(encoderData.angle-Encoder_VertPos-90+Deg_Ball_to_Arm)*Rotation_r_Of_Ball;
        
        //模型预测
        if(my_Shoot_Task_T.shoot_point == 7)
        {
            my_Shoot_Task_T.model_calc_KW = Calc_KW(my_Shoot_Task_T.Auto_shoot_point[my_Shoot_Task_T.shoot_point].distance)
                                         + MyRemote_Data.KW00001*2 + MyRemote_Data.KW_00001 + MyRemote_Data.btn_RightCrossRight_press_count*0.0001 ;
            my_Shoot_Task_T.model_calc_degree = Calc_Degree(my_Shoot_Task_T.Auto_shoot_point[my_Shoot_Task_T.shoot_point].distance);
                                             
            my_Shoot_Task_T.camera_yaw_turning = 5.8 + MyRemote_Data.btn_LeftCrossLeft_press_count*0.1 
                                                    - MyRemote_Data.btn_LeftCrossRight_press_count*0.1;
            if(my_Shoot_Task_T.model_calc_KW >= 0.92)
            {
                my_Shoot_Task_T.model_calc_KW = 0.92;
            }
        }else if (my_Shoot_Task_T.shoot_point == 1)
        {
            my_Shoot_Task_T.model_calc_KW = Calc_KW(my_Shoot_Task_T.Auto_shoot_point[7].distance) + MyRemote_Data.KW00001 + MyRemote_Data.KW_00001;
            my_Shoot_Task_T.model_calc_degree = Calc_Degree(my_Shoot_Task_T.Auto_shoot_point[7].distance);

            my_Shoot_Task_T.camera_yaw_turning = 5.8 + MyRemote_Data.btn_LeftCrossLeft_press_count*0.1 
                                                    - MyRemote_Data.btn_LeftCrossRight_press_count*0.1;
                                             
            if(my_Shoot_Task_T.model_calc_KW >= 0.92)
            {
                my_Shoot_Task_T.model_calc_KW = 0.92;
            }
        }else{
            my_Shoot_Task_T.model_calc_KW = 0.883;
            my_Shoot_Task_T.model_calc_degree = 75;
        }
        
        
        // my_Shoot_Task_T.model_calc_KW = Calc_KW(my_Alldir_Chassis_t.chassis_to_basket) + MyRemote_Data.KW00001 + MyRemote_Data.KW_00001;
        // my_Shoot_Task_T.model_calc_degree = Calc_Degree(my_Alldir_Chassis_t.chassis_to_basket) - MyRemote_Data.btn_RightCrossRight_press_count*0.5;

        switch (my_Shoot_Task_T.myshoot_status)
        {
        case SHOOT_IDLE:
            if(myHandle_State == HANDLE_SHOOT_MODE)
            {
                my_Shoot_Task_T.Shoot_Completed_Flag = 0;

                //Ball_Hold(0.1);
                Unitree_motor_0Torque(5);
                Unitree_motor_0Torque(3);
                osDelay(1);
                if(encoderData.angle >= 270)
                {
                    my_Shoot_Task_T.myshoot_status = SHOOT_BACK_TO_INIT;
                }
            }
            break;
        case SHOOT_BACK_TO_INIT:
            Back_to_init();
            Unitree_motor_0Torque(5);
            if(encoderData.angle <= 80)
            {
                my_Shoot_Task_T.myshoot_status = SHOOT_IDLE;
            }
            break;
        case SHOOT_ING:
            //投篮控制
            if (my_Shoot_Task_T.Shoot_Completed_Flag == 0)
            {
                my_Shoot_Task_T.on_shoot_point = 0;
                my_Shoot_Task_T.shoot_count++;
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                // unitree_DunkMotor_t[3].cmd.K_W = my_Alldir_Chassis_t.test_KW;
                unitree_DunkMotor_t[3].cmd.K_W = my_Shoot_Task_T.model_calc_KW;
                unitree_DunkMotor_t[3].cmd.W = -1;//-1
                unitree_DunkMotor_t[3].cmd.T = 0;//-3.1
                
                unitree_DunkMotor_t[5].cmd.Pos = 0;
                unitree_DunkMotor_t[5].cmd.K_P = 0;
                // unitree_DunkMotor_t[5].cmd.K_W = my_Alldir_Chassis_t.test_KW;
                unitree_DunkMotor_t[5].cmd.K_W = my_Shoot_Task_T.model_calc_KW;
                unitree_DunkMotor_t[5].cmd.W = 1;
                unitree_DunkMotor_t[5].cmd.T = 0;//3.1
                my_Shoot_Task_T.Shoot_Completed_Flag = 1;
            }

            //刹停力矩
            if(my_Shoot_Task_T.Shoot_Completed_Flag==1 && encoderData.angle >= Encoder_VertPos + my_Shoot_Task_T.model_calc_degree )
            {
                unitree_DunkMotor_t[3].cmd.Pos = 0;
                unitree_DunkMotor_t[3].cmd.K_P = 0;
                // unitree_DunkMotor_t[3].cmd.K_W = my_Alldir_Chassis_t.test_KW;
                unitree_DunkMotor_t[3].cmd.K_W = my_Shoot_Task_T.model_calc_KW;
                unitree_DunkMotor_t[3].cmd.W = 1;
                unitree_DunkMotor_t[3].cmd.T = 0;

                unitree_DunkMotor_t[5].cmd.Pos = 0;
                unitree_DunkMotor_t[5].cmd.K_P = 0;
                // unitree_DunkMotor_t[5].cmd.K_W = my_Alldir_Chassis_t.test_KW;
                unitree_DunkMotor_t[5].cmd.K_W = my_Shoot_Task_T.model_calc_KW;
                unitree_DunkMotor_t[5].cmd.W = -1;
                unitree_DunkMotor_t[5].cmd.T = 0;
                my_Shoot_Task_T.Shoot_Completed_Flag =2;
                
            }
            
            //下降控制
            if (encoderData.angle <= Encoder_VertPos + my_Shoot_Task_T.model_calc_degree - 4 && my_Shoot_Task_T.Shoot_Completed_Flag == 2 )
            {
                if(my_Shoot_Task_T.shoot_point  == 7)
                {
                    if(my_Shoot_Task_T.shoot_count%2 == 0){    
                        unitree_DunkMotor_t[5].cmd.Pos = 0;
                        unitree_DunkMotor_t[5].cmd.K_P = 0;
                        unitree_DunkMotor_t[5].cmd.K_W = 2;
                        unitree_DunkMotor_t[5].cmd.W = -0.001;
                        unitree_DunkMotor_t[5].cmd.T = 0;
                        Unitree_motor_0Torque(3);
                    }else{
                        unitree_DunkMotor_t[3].cmd.Pos = 0;
                        unitree_DunkMotor_t[3].cmd.K_P = 0;
                        unitree_DunkMotor_t[3].cmd.K_W = 2;
                        unitree_DunkMotor_t[3].cmd.W = 0.001;
                        unitree_DunkMotor_t[3].cmd.T = 0;
                        Unitree_motor_0Torque(5);
                    }
                }else{
                    unitree_DunkMotor_t[5].cmd.Pos = my_Shoot_Task_T.Shootball_InitialPos[1]  + 1;
                    unitree_DunkMotor_t[5].cmd.K_P = 0.4;
                    unitree_DunkMotor_t[5].cmd.K_W = 0.15;
                    unitree_DunkMotor_t[5].cmd.W = -0.2;
                    unitree_DunkMotor_t[5].cmd.T = 0;
                    Unitree_motor_0Torque(3);
                }


                // my_Shoot_Task_T.camera_get_angle_flag = 0;
                if(encoderData.angle <= Encoder_VertPos + 3)
                {
                    my_Shoot_Task_T.Shoot_Completed_Flag = 3;
                    // Unitree_motor_0Torque(5);
                    // Unitree_motor_0Torque(3);
                }

            }

            if(my_Shoot_Task_T.Shoot_Completed_Flag == 3&&encoderData.angle <= Encoder_VertPos + 1)
            {
                //Ball_Hold(-0.1);
                // Unitree_motor_0Torque(5);
                // Unitree_motor_0Torque(3);
                my_Shoot_Task_T.myshoot_status = SHOOT_IDLE;
            }
            osDelay(1);
            break;
        case SHOOT_FINISH :
            my_Shoot_Task_T.myshoot_status = SHOOT_IDLE;
            break;    
        default:
            break;
        }

        osDelay(2);
    }
    

}