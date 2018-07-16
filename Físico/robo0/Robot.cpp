#include <math.h>
#include "Robot.hpp"
int myABS(int x)
{
    return (x>0? x : -x);
}

Robot::Robot()
{
    int i;
    motor = new Motor* [2];
    ultrasonic = new MyUltrasonic* [N_ULTRASONIC];
    black_tape_sensor = new BlackTapeSensor* [N_BLACK_TAPE_SENSOR];
    /*for(i = 0; i<2; i++)
    {
        motor[i] = new Motor(i+1);
    }*/
    motor[0] = new Motor(L_MOTOR);
    motor[1] = new Motor(R_MOTOR);
    for(i=0; i<N_ULTRASONIC; i++)
    {
        ultrasonic[i] = new MyUltrasonic(i+1);
    }
    for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
    {
        black_tape_sensor[i] = new BlackTapeSensor(i+1);
    }
    lm_speed = MOTORSPEED;
    rm_speed = MOTORSPEED*SCALE;
}

void Robot::moveFoward()
{
    int i, turnright, turnleft;
    long time = millis(), timeold = millis(), time_stop;
    int* readingBTS = new int [N_BLACK_TAPE_SENSOR];
    int* readingU = new int [N_ULTRASONIC];
    //while(millis()-time<5000)
    //{
    //    motor[0]->move(true, lm_speed);
    //    motor[1]->move(true, rm_speed);
    //    moveStraight(&timeold);
    //}
    
    //while(1)
    //{
        //for(i=0; i<N_ULTRASONIC; i++)
        //{
            //Serial.print("ultrasonic ");
            //Serial.print(i);
            //Serial.print(": ");
          //  readingU[i] = (ultrasonic[i]->isDistanceAvailable() > OBSTACLE_DIS ? 0 : 1);
            //Serial.println(ultrasonic[i]->isDistanceAvailable());
            
        //}
        //Serial.println();
    //}


//    while(1)//(millis()-time<=5000))
//    {
        
        //motor[0]->move(true, lm_speed);
        //motor[1]->move(true, rm_speed);
        //moveStraight(&timeold);
        
     

        for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
        {
            readingBTS[i] = black_tape_sensor[i]->getReading();
        }
        //for(i=0; i<N_ULTRASONIC; i++)
        //{
            //readingU[i] = (ultrasonic[i]->isDistanceAvailable() > OBSTACLE_DIS ? 0 : 1);    
        //}
        readingU[2] = (ultrasonic[2]->isDistanceAvailable() > OBSTACLE_DIS ? 0 : 1);
        //Serial.println(!//readingU[2]);
        

    //}
        while(((readingBTS[0]&&readingBTS[2])||(readingBTS[2]&&readingBTS[4])) && !readingU[2])//verificar se ta no mesmo estado, tecnicamente inutil devido a funcao acima
        {
            //Serial.print(" v0: ");
            //Serial.print(lm_speed);
            //Serial.print(" v1: ");
            //Serial.println(rm_speed);
            motor[0]->move(true, lm_speed);
            motor[1]->move(true, rm_speed);
            moveStraight(&timeold);

            for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
            {
                readingBTS[i] = black_tape_sensor[i]->getReading();
            }
            //for(i=0; i<N_ULTRASONIC; i++)
            //{
            //    //readingU[i] = (ultrasonic[i]->isDistanceAvailable() > OBSTACLE_DIS ? 0 : 1);
                //Serial.print(//readingU[i]);    
            //}
            //readingU[2] = (ultrasonic[2]->isDistanceAvailable() > OBSTACLE_DIS ? 0 : 1);
           // Serial.println("to lendo todo mundo!!");
            
        }
        while(!((readingBTS[0]&&readingBTS[2])||(readingBTS[2]&&readingBTS[4])) && !readingU[2])//anda ate os sensores captarem a linha ou obstaculo
        {
            for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
            {
                readingBTS[i] = black_tape_sensor[i]->getReading();
            }
            //for(i=0; i<N_ULTRASONIC; i++)
            //{
            //    //readingU[i] = (ultrasonic[i]->isDistanceAvailable() > OBSTACLE_DIS ? 0 : 1);    
            //}
            //readingU[2] = (ultrasonic[2]->isDistanceAvailable() > OBSTACLE_DIS ? 0 : 1);
            //Serial.println("to achando o caminho!!");
            motor[0]->move(true, lm_speed);
            motor[1]->move(true, rm_speed);
                
            if(readingBTS[4]&&!(readingBTS[2]||readingBTS[1]||readingBTS[0]))
            {
                //Serial.println("vou parar o motor 1");
                //motor[0]->move(true, lm_speed);
                //motor[1]->move(false, rm_speed);
                //motor[1]->move(true, rm_speed/2);
                //motor[1]->stop();
                reduceSpeed(&timeold, 1, 2);
                delay(UPDATE_DELAY);
                for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
                {
                    readingBTS[i] = black_tape_sensor[i]->getReading();
                }
                turnright = 1;
                turnleft = 0;
            }
            else if(readingBTS[3]&&!(readingBTS[2]||readingBTS[1]||readingBTS[0]))
            {
                //motor[0]->move(true, lm_speed);
                //motor[1]->move(false, rm_speed);
                //motor[1]->move(true, rm_speed/1.5);
                //motor[1]->stop();
                reduceSpeed(&timeold, 1, 1);
                delay(UPDATE_DELAY);
                for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
                {
                    readingBTS[i] = black_tape_sensor[i]->getReading();
                }
                turnright = 1;
                turnleft = 0;
            }
            else if(readingBTS[1]&&!(readingBTS[2]||readingBTS[3]||readingBTS[4]))//robo ta desviando para a direita, gira esquerda
            {
                //motor[0]->move(false, lm_speed);
                //motor[0]->move(true, lm_speed/1.5);
                //motor[0]->stop();
                //motor[1]->move(true, rm_speed);
                reduceSpeed(&timeold, 0, 1);
                delay(UPDATE_DELAY);
                for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
                {
                    readingBTS[i] = black_tape_sensor[i]->getReading();
                }
                turnright = 0;
                turnleft = 1;
            }
            else if(readingBTS[0]&&!(readingBTS[2]||readingBTS[3]||readingBTS[4]))//robo ta desviando para a direita, gira esquerda
            {
                //motor[0]->move(false, lm_speed);
               // motor[0]->move(true, lm_speed/2);
                //motor[0]->stop();
                //motor[1]->move(true, rm_speed);
                reduceSpeed(&timeold, 0, 2);
                delay(UPDATE_DELAY);
                for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
                {
                    readingBTS[i] = black_tape_sensor[i]->getReading();
                }
                turnright = 0;
                turnleft = 1;
            }
            else if(!(readingBTS[0]||readingBTS[1]||readingBTS[2]||readingBTS[3]||readingBTS[4]))
            {
                if(turnright)
                {
                    //motor[0]->move(true, lm_speed);
                    //motor[1]->move(true, rm_speed/8);
                    //motor[1]->stop();
                    reduceSpeed(&timeold, 1, 4);
                    delay(UPDATE_DELAY);
                    for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
                    {
                        readingBTS[i] = black_tape_sensor[i]->getReading();
                    }
                }
                else if(turnleft)
                {
                    //motor[0]->move(true, lm_speed/8);
                    //motor[0]->stop();
                    //motor[1]->move(true, rm_speed);
                    reduceSpeed(&timeold, 0, 4);
                    delay(UPDATE_DELAY);
                    for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
                    {
                        readingBTS[i] = black_tape_sensor[i]->getReading();
                    }
                }
            }
            else
            {
                //motor[0]->move(true, lm_speed);
                //motor[1]->move(true, rm_speed);
                moveStraight(&timeold);
            }

           
        }
        time = millis();
        while(millis()-time <= TIMETURNING && !readingU[2])
        {
            motor[0]->move(true, lm_speed);
            motor[1]->move(true, rm_speed);
            moveStraight(&timeold);
            //for(i=0; i<N_ULTRASONIC; i++)
            //{
            //    //readingU[i] = (ultrasonic[i]->isDistanceAvailable() > OBSTACLE_DIS ? 0 : 1);    
            //}
            //readingU[2] = (ultrasonic[2]->isDistanceAvailable() > OBSTACLE_DIS ? 0 : 1);
            //Serial.println("to chegando");
        }
        delete readingU;
        delete readingBTS;
}

void Robot::turnLeft()
{
    long time = millis();
    int i;
    int* readingBTS = new int [N_BLACK_TAPE_SENSOR];
    while(millis()-time<=TIMETURNING*2)
    {
        motor[0]->move(false, lm_speed);
        motor[1]->move(true, rm_speed);
    }
    for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
    {
        readingBTS[i] = black_tape_sensor[i]->getReading();
    }
    while(readingBTS[3])
    {
        motor[0]->move(false, lm_speed);
        motor[1]->move(true, rm_speed);
        readingBTS[3] = black_tape_sensor[3]->getReading();
    }
    readingBTS[2] = black_tape_sensor[2]->getReading();
    while(!readingBTS[2])
    {
        motor[0]->move(false, lm_speed);
        motor[1]->move(true, rm_speed);
        readingBTS[2] = black_tape_sensor[2]->getReading();
    }
    delete readingBTS;
}

void Robot::turnRight()
{
    long time = millis();
    
    int i;
    int* readingBTS = new int [N_BLACK_TAPE_SENSOR];
    while(millis()-time<=TIMETURNING*2)
    {
        motor[0]->move(true, lm_speed);
        motor[1]->move(false, rm_speed);
    }
    for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
    {
        readingBTS[i] = black_tape_sensor[i]->getReading();
    }
    while(readingBTS[1])
    {
        motor[0]->move(true, lm_speed);
        motor[1]->move(false, rm_speed);
        readingBTS[1] = black_tape_sensor[1]->getReading();
    }
    readingBTS[2] = black_tape_sensor[2]->getReading();
    while(!readingBTS[2])
    {
        motor[0]->move(true, lm_speed);
        motor[1]->move(false, rm_speed);
        readingBTS[2] = black_tape_sensor[2]->getReading();
    }
    delete readingBTS;
}

void Robot::stop()
{
    motor[0]->stop();
    motor[1]->stop();
}

void Robot::moveStraight(long *timeold)
{
    int error = 0;
    int* readingRPM = new int [2];
    readingRPM[0] = motor[0]->getRPM(*timeold);
    readingRPM[1] = motor[1]->getRPM(*timeold);
    if(millis()-*timeold>=ENCODER_UPDATE_TIME)
    {
        Serial.print("rpm[0]: ");
        Serial.print(readingRPM[0]);
        Serial.print(" rpm[1]: ");
        Serial.print(readingRPM[1]);
        //Serial.print(" leitura velocidade: ");
        //Serial.print(SPEED_TO_UPDATE_TIME);
        if(readingRPM[0]>SPEED_TO_UPDATE_TIME)
        {
            lm_speed -= D_SPEED;

            error = SPEED_TO_UPDATE_TIME-readingRPM[0];
            //lm_speed -= myABS(error);
            //Serial.print("cara menor: ");
            if(lm_speed < 0)
            {
                lm_speed = 0;
            }
        }
        else
        {
            error = SPEED_TO_UPDATE_TIME-readingRPM[0];
            //Serial.print("leitura encoder esquerdo: ");
            //Serial.println(readingRPM[0]);
            //Serial.print("leitura velocidade: ");
            //Serial.println(SPEED_TO_UPDATE_TIME);
            //Serial.print("velocidade - encoder: ");
            //Serial.println(SPEED_TO_UPDATE_TIME - readingRPM[0]);
            //Serial.print("myABS da subtração: ");
            //Serial.println(myABS(SPEED_TO_UPDATE_TIME - readingRPM[0]));
            
            
            //Serial.print(int(SPEED_TO_UPDATE_TIME-readingRPM[0]));
            lm_speed += D_SPEED;
            //lm_speed += myABS(error);
            //Serial.print("cara maior");
            if(lm_speed > 255)
            {
                lm_speed = 255;
            }
        }
        if(readingRPM[1]>SPEED_TO_UPDATE_TIME)
        {
            error = int(SPEED_TO_UPDATE_TIME)-readingRPM[1];
            rm_speed -= D_SPEED;
            //rm_speed -= myABS(error);
            //Serial.print("cara menor: ");
            if(rm_speed < 0)
            {
                rm_speed = 0;
            }
        }
        else
        {
            error = int(SPEED_TO_UPDATE_TIME)-readingRPM[1];
            rm_speed += D_SPEED;
            //rm_speed += myABS(error);    
            //Serial.print("cara maior: ");            
            if(rm_speed > 255)
            {
                rm_speed = 255;
            }
        }
        readingRPM[0] = motor[0]->getRPM(*timeold);
        readingRPM[1] = motor[1]->getRPM(*timeold);
        Serial.print(" v0: ");
        Serial.print(lm_speed);
        Serial.print(" v1: ");
        Serial.println(rm_speed);
        //Serial.print(millis()-*timeold);
        *timeold = millis();
        //Serial1.write("v0: ");
        //char oi[4] = {lm_speed, readingRPM[0], rm_speed, readingRPM[1]};
        //Serial1.write(oi, 4);
        //Serial1.write("\n");
    }
    delete readingRPM;
}

void Robot::reduceSpeed(long *timeold, int motor_num, float reason)
{
    int* readingRPM = new int [2];
    readingRPM[0] = motor[0]->getRPM(*timeold);
    readingRPM[1] = motor[1]->getRPM(*timeold);
    motor[motor_num]->stop();
    if(millis()-*timeold>=ENCODER_UPDATE_TIME)
    {
        if (motor_num == 1)
        {
            if(readingRPM[0]>SPEED_TO_UPDATE_TIME)
            {
                lm_speed -= D_SPEED;
                if(lm_speed < 0)
                {
                    lm_speed = 0;
                }
            }
            else
            {
                lm_speed += D_SPEED;
                if(lm_speed > 255)
                {
                    lm_speed = 255;
                }
            }
            
            /*if(readingRPM[1]>(SPEED_TO_UPDATE_TIME-reason))
            {
                rm_speed -= D_SPEED;
                if(rm_speed < 0)
                {
                    rm_speed = 0;
                }
            }
            else
            {
                rm_speed += D_SPEED;
            }*/
        }
        else
        {
            /*if(readingRPM[0]>(SPEED_TO_UPDATE_TIME-reason))
            {
                lm_speed -= D_SPEED;
                if(lm_speed < 0)
                {
                    lm_speed = 0;
                }
            }
            else
            {
                lm_speed += D_SPEED;
            }*/
            if(readingRPM[1]>SPEED_TO_UPDATE_TIME)
            {
                rm_speed -= D_SPEED;
                if(rm_speed < 0)
                {
                    rm_speed = 0;
                }
            }
            else
            {
                rm_speed += D_SPEED;
                if(rm_speed > 255)
                {
                    rm_speed = 255;
                }
            }
        }
        readingRPM[0] = motor[0]->getRPM(*timeold);
        readingRPM[1] = motor[1]->getRPM(*timeold);
        *timeold = millis();
    }
    delete readingRPM;
}

uint8_t* Robot::getReadingBlackTypeSensor()
{
    int i;
    uint8_t* readingBTS = new uint8_t [N_BLACK_TAPE_SENSOR];
    for(i=0; i<N_BLACK_TAPE_SENSOR; i++)
    {
        readingBTS[i] = black_tape_sensor[i]->getReading();
    }
    return readingBTS;
}   

float* Robot::getReadingUltrasonic()
{
    int i;
    float* detectedObjet_U = new float [N_ULTRASONIC];
    for(i=0; i<N_ULTRASONIC; i++)
    {
        detectedObjet_U[i] = ultrasonic[i]->isDistanceAvailable();
        Serial.print(detectedObjet_U[i]);
    }
    Serial.println();
    return detectedObjet_U;
}
