/***
This is the SCUC module for the project of Northwest.
In this version, replace the demand and sysDemand with Demand[d][t], and modifie the input data about load.

By Chunting
2014-10-12

**/
#include <ilcplex/ilocplex.h>
#include <fstream>
#include <math.h>
#include <ilconcert/ilomodel.h>

/************************ Define all the files ***************************************************/
#define OUTFILEDATA "./New_Output/Check.dat"    
#define OUTFILERESULT "./New_Output/Result.dat"
#define lp "./New_Output/Model.lp"     
#define SYSTEMDATA "./New_Input/SystemData.dat"   
#define APPDATA "./New_Input/AppData.dat"    
#define THUNITDATA "./New_Input/ThUnitData.dat"
#define NETDATA "./New_Input/NetData.dat"
#define GAMADATA "./New_Input/GamaData.dat"
#define HYDDATA "./New_Input/HydData.dat"		
#define WINDDATA "./New_Input/WindData.dat"		
#define SOLARDATA "./New_Input/SolarData.dat"
/************************ File End ***************************************************************/

ILOSTLBEGIN
/***************** typedefΪ IloArray<IloNumArray> �����Matrix2���Ա��ں�������ʱ����д**********/
typedef IloArray<IloNumArray> Matrix2; 
typedef IloArray<Matrix2> Matrix3;
typedef IloArray<Matrix3> Matrix4;
typedef IloArray<Matrix4> Matrix5;

typedef IloArray<IloNumVarArray>VarMatrix2;
typedef IloArray<VarMatrix2>VarMatrix3;
typedef IloArray<VarMatrix3>VarMatrix4;
typedef IloArray<VarMatrix4>VarMatrix5;
typedef IloArray<VarMatrix5>VarMatrix6;

/************************ Define Global Variables *********************************************************/
IloInt cycle; //����ʱ����
IloInt demandNum; //���ɽڵ����
IloInt lineNum; //�����߸���
IloInt busNum;  //�ڵ����
IloInt outputNum; //���ͽڵ����
IloInt sectionNum; //�������
IloInt thUnitNum; //������̨��
IloInt hyUnitNum; //ˮ�����̨��
IloInt wFieldNum; //�糡����
IloInt sPlantNum; //����糧����

const double _INF=1E-7;//add by hx

//************************************
// Method:    readSystemData
// FullName:  readSystemData
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: IloEnv env
// Parameter: IloInt & cycle, time periods,24;
// Parameter: IloInt & demandNum,demandNum, number of load, 11;
// Parameter: IloInt & lineNum,number of transmission line, 39;
// Parameter: IloInt & busNum, number of bus, 31;
// Parameter: IloInt & sectionNum,numberof section, 1 
// Parameter: IloInt & thUnitNum, number of thermal unit, 16;
// Parameter: IloInt & hyUnitNum, number of hydroelectric station
// Parameter: IloInt & wFieldNum, number of wind field, 3;	
// Parameter: IloInt & sPlantNum, number of solar farm
//************************************
void readSystemData(IloEnv env,
					IloInt& cycle,
					IloInt& demandNum,
					IloInt& lineNum,
					IloInt& busNum,
					IloInt& outputNum,
					IloInt& sectionNum,
					IloInt& thUnitNum,
					IloInt& hyUnitNum, 
					IloInt& wFieldNum,
					IloInt& sPlantNum
					)
{
	ifstream fin(SYSTEMDATA,ios::in);
	if(!fin) 
		env.out()<<"problem with file:" << SYSTEMDATA<<endl;
	fin >> cycle;
	fin >> demandNum;
	fin >> lineNum;
	fin >> busNum;
	fin >> outputNum;
	fin >> sectionNum;   //0
	fin >> thUnitNum;		// 16
	fin >> hyUnitNum;		// 1
	fin >> wFieldNum;		// 1
	fin >> sPlantNum;		// 1
	fin.close();
	env.out()<<"System date is done"<<endl;
}

/********************* readNetData by Xuan ****************************************/


//************************************
// Method:    readNetData
// FullName:  readNetData
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: IloEnv env
// Parameter: IloNumArray & unitLocation	Thermal unit����bus��� 
// Parameter: IloNumArray & demandLocation ��������bus���
// Parameter: IloNumArray & demand	����������ϵ��
// Parameter: IloNumArray & sysDemand	ϵͳ��ÿ��ʱ�̵��ܸ���
// Parameter: IloNumArray & lineCap	��������
// Parameter: IloNumArray & upSectionCap	���湦������
// Parameter: IloNumArray & downSectionCap	���湦������
// Parameter: IloNumArray & linSectionNum	ÿ�����������·����
// Parameter: Matrix2 & linSection	ÿ����������������·
//************************************
void readNetData(IloEnv env,
				 IloNumArray& unitLocation,
				 IloNumArray& demandLocation,            	
				 Matrix2& Demand,
				 IloNumArray& lineCap,	
				 IloNumArray& outputLocation, 
				 IloNumArray& upSectionCap,				
				 IloNumArray& downSectionCap,				 
				 IloNumArray& linSectionNum,				
				 Matrix2& linSection				
				 )
{
	ifstream fin(NETDATA,ios::in);
	if(!fin) env.out()<<"problem with file:" << NETDATA <<endl;
	int i,j,t,d;
	for(i=0;i<thUnitNum;i++)
	{
		fin>> unitLocation[i];
	}
	for(i=0;i<demandNum;i++)
	{
		fin>>demandLocation[i];
	}
	for(d=0; d<demandNum;d++)
	{
		for(t=1; t<cycle+1; t++)
			fin >> Demand[d][t];
	}

	for(i=0; i<lineNum; i++)
	{
		fin >> lineCap[i];
	}
	for(i=0;i< outputNum;i++)
	{
		fin>> outputLocation[i];
	}
	for(i=0; i<sectionNum; i++)
	{
		fin >> upSectionCap[i];
	}

	for(i=0; i<sectionNum; i++)
	{
		fin >> downSectionCap[i];
	}

	for(i=0; i<sectionNum; i++)
	{
		fin >> linSectionNum[i];
	}

	for(i=0;i<sectionNum;i++)
	{
		for(j=0;j<linSectionNum[i];j++)
		{
			fin>>linSection[i][j];
		}
	}
	fin.close();
	env.out()<<"Net Data is done "<<endl;
}
/*********************** End readNetData by Xuan *********************************/

//************************************
// Method:    readGama
// FullName:  readGama
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: char * gamgadata
// Parameter: IloEnv env
// Parameter: Matrix2 & gama
//************************************
int readGama(char* gamgadata,IloEnv env,Matrix2& gama)
{
	int i,j;
	ifstream fin(gamgadata,ios::in);		
	if(!fin) 
		env.out()<<"problem with "<< gamgadata<<endl;
	//read gama
	for(i=0;i<lineNum;i++)
	{
		for(j=0;j<busNum;j++)
		{
			fin>>gama[i][j];
		}
	}
	return 0;
}


//************��ȡ���������ݺ���************************
//************************************
// Method:    readThUnitData
// FullName:  readThUnitData
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: char * thunitdata
// Parameter: IloEnv env
// Parameter: IloNumArray & thminPower
// Parameter: IloNumArray & thmaxPower
// Parameter: IloNumArray & thminDown
// Parameter: IloNumArray & thminUp
// Parameter: IloNumArray & thcoldUpTime
// Parameter: IloNumArray & thfuelCostPieceNum
// Parameter: IloNumArray & thhotUpCost
// Parameter: IloNumArray & thcoldUpCost
// Parameter: IloNumArray & thdelta
// Parameter: IloNumArray & thfirstLast
// Parameter: IloNumArray & thmaxR
// Parameter: IloNumArray & tha
// Parameter: IloNumArray & thb
// Parameter: Ilo
//************************************
int readThUnitData(char* thunitdata,
				   IloEnv env,                                   //���廷������
				   IloNumArray& thminPower,                        //������С������
				   IloNumArray& thmaxPower,                        //������󷢵���
				   IloNumArray& thminDown,                         //������С�ػ�ʱ��
				   IloNumArray& thminUp,                           //������С����ʱ��
				   IloNumArray& thcoldUpTime,                      //������ʱ��
				//   IloNumArray& thfuelCostPieceNum,                //ȼ�Ϸ������߶���
				   IloNumArray& thhotUpCost,                       //����������
				   IloNumArray& thcoldUpCost,			           //���������� IloNumArray& hotUpTime
				   IloNumArray& thdelta,						   //����
				//   IloNumArray& thfirstLast,                       //��ĩ����Լ����ȡ0��1
				   IloNumArray& thmaxR,                            //���������	
				 //  IloNumArray& tha,							   //ȼ�Ϸ���������ϵ��a		
				   IloNumArray& thb,							   //ȼ�Ϸ���������ϵ��b		
				   IloNumArray& thc,						       //ȼ�Ϸ���������ϵ��c
				   IloNumArray& thminFuelCost,                        //������С�������
				   IloNumArray& thmaxFuelCost,                        //������󷢵����
				   IloNumArray& thinitState,                        //�����ʼ״̬
				   IloNumArray& thinitPower                        //�����ʼ������			 
				   )
{
	ifstream fin(thunitdata,ios::in);
	if(!fin) env.out()<<"problem with file:"<<thunitdata<<endl;
	int i;

	//read minPower
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thminPower[i];
	}

	//read maxPower
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thmaxPower[i];
	}

	//read minDown
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thminDown[i];
	}

	//read minUp
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thminUp[i];
	}

	//read coldUpTime
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thcoldUpTime[i];
	}

	//read thfuelCostPieceNum
	/*
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thfuelCostPieceNum[i];
	}
*/
	//read hotUpCost
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thhotUpCost[i];
	}

	//read coldUpCost
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thcoldUpCost[i];
	}

	//read delta
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thdelta[i];
	}

	//read firstlast
	/*
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thfirstLast[i];
	}
	*/

	//read maxR
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thmaxR[i];
	}

	//read b
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thb[i];
	}
	//read c
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thc[i];
	}

	for(i=0;i<thUnitNum;i++)
	{
	//	thminFuelCost[i]=tha[i]*thminPower[i]*thminPower[i]+thb[i]*thminPower[i]+thc[i];
	//	thmaxFuelCost[i]=tha[i]*thmaxPower[i]*thmaxPower[i]+thb[i]*thmaxPower[i]+thc[i];
		thminFuelCost[i]=thb[i]*thminPower[i]+thc[i];
		thmaxFuelCost[i]=thb[i]*thmaxPower[i]+thc[i];
	}
	//read thinitState
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thinitState[i];
	}
	//read thinitPower
	for(i=0;i<thUnitNum;i++)
	{
		fin>>thinitPower[i];
	}


	return 0;
}



//������ݶ�ȡ����
//************************************
// Method:    readWindData
// FullName:  readWindData
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: IloEnv env
// Parameter: IloNumArray & windLocation, the location of wind field
// Parameter: IloNumArray & maxWindPower, upper bound of wind power
// Parameter: IloNumArray & minWindPower,lower bound of wind power
//************************************
void readWindData(IloEnv env,
				  IloNumArray& windLocation,		
				  IloNumArray& maxWindPower,		
				  IloNumArray& minWindPower		
				  )
{
	ifstream fin(WINDDATA,ios::in);
	if(!fin) env.out()<<"problem with file:" << WINDDATA <<endl;
	int w = 0;

	for(w=0;w<wFieldNum;w++)
	{
		fin>>windLocation[w];
	}

	for(w=0;w<wFieldNum;w++)
	{
		fin>>maxWindPower[w];
	}

	for(w=0;w<wFieldNum;w++)
	{
		fin>>minWindPower[w];
	}
	fin.close();
	env.out()<<"Wind data is done"<<endl;
}


//ˮ�����ݶ�ȡ����
//************************************
// Method:    readHydData
// FullName:  readHydData
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: IloEnv env
// Parameter: IloNumArray & hyUnitLocation, location of hydro
// Parameter: IloNumArray & maxhyUnitPower, upper of hydropower
// Parameter: IloNumArray & minhyUnitPower, lower of hydropower
//************************************
void readHydData(IloEnv env,
				 IloNumArray& hyUnitLocation,	//ˮ�糧λ��
				 IloNumArray& maxhyUnitPower,	//ˮ���������
				 IloNumArray& minhyUnitPower		//ˮ���������
				 )
{
	ifstream fin(HYDDATA,ios::in);
	if(!fin) env.out()<<"problem with file:" << HYDDATA <<endl;
	int k = 0;

	for(k=0;k<hyUnitNum;k++)
	{
		fin>>hyUnitLocation[k];
	}

	for(k=0;k<hyUnitNum;k++)
	{
		fin>>maxhyUnitPower[k];
	}

	for(k=0;k<hyUnitNum;k++)
	{
		fin>>minhyUnitPower[k];
	}
	fin.close();
	env.out()<<"Hydro data is done"<<endl;
}

//������ݶ�ȡ����
//************************************
// Method:    readSolarData
// FullName:  readSolarData
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: IloEnv env
// Parameter: IloNumArray & sPlantLocation, location of Solar Power Plant
// Parameter: Matrix2 & maxSPlantPower, upper bound of Solar Power Plant
// Parameter: Matrix2 & minSPlantPower, lower bound of Solar Power Plant
//************************************
void readSolarData(IloEnv env,
				   IloNumArray& sPlantLocation,	//��糧λ��
				   Matrix2& maxSPlantPower,	//��糧���Ԥ�����
				   Matrix2& minSPlantPower		//��糧���Ԥ�����
				   )
{
	ifstream fin(SOLARDATA,ios::in);
	if(!fin) env.out()<<"problem with file:" << SOLARDATA <<endl;
	int i, t;

	for(i=0;i<sPlantNum;i++)
	{
		fin>>sPlantLocation[i];
	}

	for(i=0;i<sPlantNum;i++)
	{
		for(t=1;t<cycle+1;++t)
		{
			fin>>maxSPlantPower[i][t];
		}
	}

	for(i=0;i<sPlantNum;i++)
	{
		for(t=1;t<cycle+1;++t)
		{
			fin>>minSPlantPower[i][t];
		}
	}
	fin.close();
	env.out()<<"Solar data is done"<<endl;
}

int main(int argc, char *argv[])
{

	IloEnv env;
	int i,j,l,d,day;
	int s = 0;
	int w = 0;
	int k = 0;
	int t = 0;
	int o = 0;
	float r= 0.15; // Pay attention to this variable, it should be input by reading file.
	try
	{
		IloModel model(env);
		IloTimer timer(env);
		timer.start();
		//*************��ȡϵͳ����***************************	
		readSystemData(env,cycle,demandNum,lineNum,busNum,outputNum, sectionNum,thUnitNum,hyUnitNum,wFieldNum,sPlantNum);

		ifstream fin(APPDATA,ios::in);
		if(!fin) 
			env.out()<<"problem with file:"<<APPDATA<<endl;

		//IloNumArray sysDemand(env,cycle+1);
		IloNumArray sysReserve(env,cycle+1);
		Matrix2 windPower(env,wFieldNum);
		Matrix2 hyPower(env,hyUnitNum);
		Matrix2 outputPower(env,outputNum);
		for (w=0; w<wFieldNum; ++w)
		{
			windPower[w] = IloNumArray(env,cycle+1);
		}
		for (k=0; k<hyUnitNum; ++k)
		{
			hyPower[k] = IloNumArray(env,cycle+1);
		}
		for (o=0; o<outputNum; ++o)
		{
			outputPower[o] = IloNumArray(env,cycle+1);
		}
		day=1;
		//sysReserve
		for(t=1;t<cycle+1;t++)
			fin>> sysReserve[t];
		for(w = 0; w<wFieldNum; w++)
		{
			for( t=1;t<cycle+1;++t) {
				fin >> windPower[w][t];
			}
		}
		for(k=0; k<hyUnitNum; ++k)
		{
			for( t=1;t<cycle+1;++t) {
				fin >>hyPower[k][t];
			}
		}
		for(o=0; o<outputNum; ++o)
		{
			for( t=1;t<cycle+1;++t) {
				fin >> outputPower[o][t];
			}
		}
		env.out() << "APPDATA is ok !" << endl;
		//*************��ȡ�������ݣ�����070411**************************		
		IloNumArray unitLocation(env,thUnitNum);
		IloNumArray demandLocation(env,demandNum);
		IloNumArray outputLocation(env,outputNum);
		//IloNumArray demand(env,demandNum);
		Matrix2 Demand(env, demandNum);				//�޸ĺ�ĸ��ɸ�Ϊ��ά����
		IloNumArray lineCap(env,lineNum);	
		IloNumArray upSectionCap(env,sectionNum);	//���湦������
		IloNumArray downSectionCap(env,sectionNum);	//���湦������
		IloNumArray linSectionNum(env,sectionNum);	//��������·�ĸ���
		Matrix2 linSection(env,sectionNum);			//�����еľ�����·,����[�����][��·��]
		for(int i_sec=0; i_sec<sectionNum; i_sec++)
		{
			linSection[i_sec]=IloNumArray(env,linSectionNum[i_sec]);
		}
		for(d=0;d<demandNum; d++)
		{	
			Demand[d] = IloNumArray( env, cycle+1 );
		}
		readNetData(env,unitLocation, demandLocation,
			//demand,sysDemand,
			Demand,
			lineCap,outputLocation,upSectionCap,downSectionCap,linSectionNum,linSection);	



		//*************����������**************************
		IloNumArray thminPower(env,thUnitNum);                                    //��С������
		IloNumArray thmaxPower(env,thUnitNum);                                    //��󷢵���
		IloNumArray thminFuelCost(env,thUnitNum);
		IloNumArray thmaxFuelCost(env,thUnitNum);
		IloNumArray thminUp(env,thUnitNum);                                       //��С����ʱ��
		IloNumArray thminDown(env,thUnitNum);                                     //��С�ػ�ʱ��
		IloNumArray thcoldUpTime(env,thUnitNum);
		IloNumArray thfuelCostPieceNum(env,thUnitNum);
		IloNumArray thhotUpCost(env,thUnitNum);
		IloNumArray thcoldUpCost(env,thUnitNum);
		IloNumArray thdelta(env,thUnitNum);			
	//	IloNumArray thfirstLast(env,thUnitNum);
		IloNumArray thmaxR(env,thUnitNum);
	//	IloNumArray tha(env,thUnitNum);
		IloNumArray thb(env,thUnitNum);
		IloNumArray thc(env,thUnitNum);
		IloNumArray thinitState(env,thUnitNum);
		IloNumArray thinitPower(env,thUnitNum);



		//*************�����ȡ��������***************************
		readThUnitData(THUNITDATA,env,
			thminPower,                        //������С������
			thmaxPower,                        //������󷢵���
			thminDown,                         //������С�ػ�ʱ��
			thminUp,                           //������С����ʱ��
			thcoldUpTime,                      //������ʱ��
		//	thfuelCostPieceNum,                //ȼ�Ϸ������߶���
			thhotUpCost,                       //����������
			thcoldUpCost,			           //����������
			thdelta,						      //����
		//	thfirstLast,                       //��ĩ����Լ����ȡ0��1
			thmaxR,                          //���������	
			//tha,							     //ȼ�Ϸ���������ϵ��a		
			thb,							     //ȼ�Ϸ���������ϵ��b		
			thc,
			thminFuelCost,
			thmaxFuelCost,
			thinitState,
			thinitPower
			);
		//������ݶ�ȡ
		IloNumArray windLocation(env,wFieldNum);		//��糡λ��
		IloNumArray maxWindPower(env,wFieldNum);		//��糡��������
		IloNumArray minWindPower(env,wFieldNum);		//��糡��������
		readWindData(env,windLocation,maxWindPower,minWindPower);		//��ȡ����

		//ˮ�����ݶ�ȡ
		IloNumArray hyUnitLocation(env,hyUnitNum);		//ˮ�����λ��
		IloNumArray maxHyUnitPower(env,hyUnitNum);		//ˮ������������
		IloNumArray minHyUnitPower(env,hyUnitNum);		//ˮ������������
		readHydData(env,hyUnitLocation,maxHyUnitPower,minHyUnitPower);	//��ȡ����

		//������ݶ�ȡ
		IloNumArray sPlantLocation(env,sPlantNum);	//��糧λ��
		Matrix2 maxSPlantPower(env,sPlantNum);		//��糧���Ԥ�����
		for(s=0;s<sPlantNum;s++)
		{
			maxSPlantPower[s] = IloNumArray(env,cycle+1);
		}
		Matrix2 minSPlantPower(env,sPlantNum);		//��糧��СԤ�����
		for(s=0; s<sPlantNum; s++)
		{
			minSPlantPower[s] = IloNumArray(env,cycle+1);
		}
		readSolarData(env,sPlantLocation,maxSPlantPower,minSPlantPower);	//��ȡ����

		//************��ȡgama*********
		Matrix2      gama(env, lineNum);
		for(i=0;i<lineNum;i++)
		{
			gama[i]=IloNumArray(env, busNum);
		}
		readGama(GAMADATA,env,gama);

		//���
		ofstream tfile(OUTFILEDATA,ios::out);
		if(!tfile)
			cout<<"cannot open "<<OUTFILEDATA<<endl;
		tfile << "cycle  " << cycle << endl;
		tfile << "demandNum  " << demandNum << endl;
		tfile << "lineNum  " << lineNum << endl;
		tfile << "busNum  " << busNum << endl;
		tfile << "outputNum  " << outputNum << endl;
		tfile << "sectionNum  " << sectionNum << endl;
		tfile << "thUnitNum  " << thUnitNum << endl;
		tfile << "hyUnitNum  " << hyUnitNum << endl;
		tfile << "wFieldNum  " << wFieldNum << endl;
		tfile << "sPlantNum  " << sPlantNum << endl;

		tfile<<endl<<"***********************��������****************"<<endl;
		tfile<<endl<<"Thermal Unit Substation "<<endl;
		for (i=0;i<thUnitNum;i++)
		{
			tfile<<unitLocation[i]<<"  ";
		}

		tfile<<endl<<"Load Substation( "<<demandNum<<" )"<<endl;
		for (i=0;i<demandNum;i++)
		{
			tfile << demandLocation[i] <<"  ";
		}
		tfile<<endl<<"Load ( "<<demandNum<<" * "<<cycle<<" )"<<endl;
		//�����߰�ȫԼ��
		for(d=0;d<demandNum;d++)
		{
			for(t=1; t<cycle+1; t++)
				tfile << Demand[d][t]<<"  ";
			tfile << endl;
		}
		tfile << endl << "line capacity"<< endl;
		for (i=0;i<lineNum;i++)
		{
			tfile << lineCap[i] << "  ";
		}
		tfile << endl << "Output Location"<< endl;
		for (i=0;i<outputNum;i++)
		{
			tfile << outputLocation[i] << "  ";
		}
/*
		tfile << endl << "up Section Cap"<< endl;
		for(i=0;i<sectionNum;i++)
		{
			tfile << upSectionCap[i] << "  ";
		}
		tfile << endl << "down Section Cap"<< endl;
		for(i=0;i<sectionNum;i++)
		{
			tfile << downSectionCap[i] << "  ";
		}
		tfile << endl << "line Section Num"<< endl;
		for(i=0;i<sectionNum;i++)
		{
			tfile << linSectionNum[i] << "  ";
		}
		tfile << endl << "line Section"<< endl;
		for(i=0;i<sectionNum;i++)
		{
			for(j=0;j<linSectionNum[i];j++)
			{
				tfile << linSection[i][j] << "  ";
			}
		}
		*/
//sysReserve
		tfile << endl << "System Reserve"<< endl;
		for(t=1;t<cycle+1;t++)
			tfile << sysReserve[t] << "  ";

		tfile << endl << "Wind Power"<< endl;
		for(w = 0; w<wFieldNum; w++)
		{
			for( t=1;t<cycle+1;++t) {
				tfile << windPower[w][t] << "  ";
			}
			tfile << endl;
		}

		tfile << endl << "Hydro Power"<< endl;
		for(k=0; k<hyUnitNum; ++k)
		{
			for( t=1;t<cycle+1;++t) {
				tfile << hyPower[k][t]<< "  ";
			}
			tfile << endl;
		}
		tfile << endl << "Output Power"<< endl;
		for(o=0; o<outputNum; ++o)
		{
			for( t=1;t<cycle+1;++t) {
				tfile << outputPower[o][t]<< "  ";
			}
			tfile << endl;
		}
		tfile<<"***************************�������************************88"<<endl;
	/*
	tfile<<endl<<"Gama: ---------Begin-------"<<endl; 
		tfile<<"busNum "<<busNum<<" linNum "<<lineNum<<endl;
		for(j=0;j<lineNum;j++)
		{
			for(i=0;i<busNum;i++)
			{
				if(i%10==0) tfile<<endl;
				tfile<<gama[j][i]<<"  ";
			}
			tfile<<endl;
		}	
		tfile<<"  ---------End----"<<endl; 
*/
		tfile<<endl<<"thminPower"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile << thminPower[i] <<" ";
		}
		tfile<<endl<<"thmaxPower"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thmaxPower[i]<<" ";
		}
		tfile<<endl<<"thminDown"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thminDown[i]<<"  ";
		}
		tfile<<endl<<"thminUp"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thminUp[i]<<"  ";
		}
		tfile<<endl<<"thcoldUpTime"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thcoldUpTime[i]<<"  ";
		}

		tfile<<endl<<"thhotUpCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thhotUpCost[i]<<"  ";
		}
		tfile<<endl<<"thcoldUpCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thcoldUpCost[i]<<" ";
		}
		tfile<<endl<<"thdelta"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thdelta[i]<<" ";
		}
		tfile<<endl<<"thmaxR"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thmaxR[i]<<" ";
		}
		tfile<<endl<<"thb"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thb[i]<<"  ";
		}
		tfile<<endl<<"thc"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thc[i]<<"  ";
		}
		tfile<<endl<<"thminFuelCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thminFuelCost[i]<<" ";
		}
		tfile<<endl<<"thmaxFuelCost"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thmaxFuelCost[i]<<" ";
		}
		tfile<<endl<<"thinitState"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thinitState[i]<<"\t";
		}
		tfile<<endl<<"thinitPower"<<endl;
		for(i=0;i<thUnitNum;i++)
		{
			tfile<<thinitPower[i]<<" ";
		}
		tfile<<endl<<"maxHyUnitPower"<<endl;
		for(k=0; k<hyUnitNum; ++k)
		{
			tfile << maxHyUnitPower[k] <<" ";
		}
		tfile<<endl<<"minHyUnitPower"<<endl;
		for(k=0; k<hyUnitNum; ++k)
		{
			tfile<<minHyUnitPower[k]<<" ";
		}

		tfile<<endl << "max Wind Power"<< endl;

		for(w = 0; w<wFieldNum; w++)
		{
			tfile << maxWindPower[w] <<" ";
		}
		tfile<<endl<<"min Wind Power"<<endl;
		for(w=0;w<wFieldNum;w++)
		{
			tfile<<minWindPower[w]<<" ";
		 }
		
		tfile.close();

		//************************����������***********************


		/************************************************************************/
		/*			Define variable of thermal unit		By Chun-Ting            */
		/*	1. State of unit, 1 is up and 0 is down;                            */
		/*	2. thermalPower[i][t], power generated by unit i at time t,in MW;   */
		/*	3. thermalR[i][t], spinning contribution of unit i at time t,in MW; */
		/*	4. startUp[i][t], action of start up of unit i at time t,0 or 1;    */
		/*	5. shutDown[i][t], action of shut down of unit i at time t,0 or 1;  */
		/*	6. upCost[i][t], startup cost of unit i at time t;					*/
		/*	7. fuelCost[i][t], fuel cost of unit i at time t;					*/
		/************************************************************************/
		VarMatrix2 state(env,thUnitNum);
		VarMatrix2 thermalPower(env,thUnitNum);
		VarMatrix2 thermalR(env,thUnitNum);
		VarMatrix2 thermalRN(env,thUnitNum);
		VarMatrix2 startUp = VarMatrix2(env,thUnitNum);
		VarMatrix2 shutDown = VarMatrix2(env,thUnitNum);
		VarMatrix2 upCost(env,thUnitNum);
		VarMatrix2 fuelCost = VarMatrix2(env,thUnitNum);
		for(i = 0; i < thUnitNum; i++)
		{
			state[i] = IloNumVarArray(env, cycle+1, 0, 1, ILOINT);
			thermalPower[i] = IloNumVarArray(env,cycle+1,0,thmaxPower[i],ILOFLOAT);
			thermalR[i] = IloNumVarArray(env,cycle+1,0,thmaxR[i],ILOFLOAT);
			thermalRN[i] = IloNumVarArray(env,cycle+1,0,thmaxR[i],ILOFLOAT);
			startUp[i] = IloNumVarArray(env, cycle+1, 0, 1, ILOINT);
			shutDown[i] = IloNumVarArray(env,cycle+1,0,1,ILOINT);	
			upCost[i] = IloNumVarArray(env,cycle+1,0,thcoldUpCost[i],ILOFLOAT);
			fuelCost[i] = IloNumVarArray(env,cycle+1,0,thmaxFuelCost[i],ILOFLOAT);
		}
		//***********************************************Լ��******************************************
		/***************  Within the multi power system, the constraint is still ok or not ? *************/
		//���н�Լ��
		/*
		for(t=1;t<cycle+1;t++)
		{
			IloExpr thsummaxp(env);
		//	IloExpr hysummaxp(env);
		//	IloExpr windsummaxp(env);
			IloExpr demandsum(env);
			for(i=0;i<thUnitNum;i++)
			{
				thsummaxp+=thmaxPower[i]*state[i][t];
			}
			for(d=0;d<demandNum; ++d)
			{
				demandsum += Demand[d][t];
			}

			model.add(thsummaxp >= demandsum + sysReserve[t]);
		}
		*/
		//���н��е�һ��Լ��
		/*
		for(t=1;t<cycle+1;t++)
		{
			IloExpr thsumminp(env);
			IloExpr demandsum(env);
			for(i=0;i<thUnitNum;i++)
			{
				thsumminp += thminPower[i]*state[i][t];
			}
			for(d=0;d<demandNum; ++d)
			{
				demandsum += Demand[d][t];
			}
		//	model.add(thsumminp<=sysDemand[t]);
			model.add(thsumminp <= demandsum);
		}
		*/
		/******************************************** End *************************************************************/
		//***********************������Լ��***********************
		//��ʼ״̬Լ��
    
		for(i = 0; i < thUnitNum; i++)
		{
			if(thinitState[i] < 0)
			{
				model.add(state[i][0] == 0);
				model.add(thermalPower[i][0] == 0);
				model.add(thermalR[i][0]==0);
			}
			else if(thinitState[i] > 0)
			{
				model.add(state[i][0] == 1);								
				model.add(thermalR[i][0]==0);
			}
		}

    
		//��ʼ���ػ�Լ��
		for(i = 0; i< thUnitNum; i++)
		{
			model.add(startUp[i][0] == 0);
			model.add(shutDown[i][0] == 0);		
		}

		//���ػ�״̬Լ������״̬ת��Լ����
		for(i = 0; i < thUnitNum; i++)
		{
			for(t = 1; t < cycle+1; t++)
			{
				model.add(state[i][t]-state[i][t-1]-startUp[i][t]+shutDown[i][t]==0);
				model.add(startUp[i][t] + shutDown[i][t]<=1);
			}
		}

		//��С���ػ�ʱ��Լ��
		for(i = 0; i < thUnitNum; i++)
		{
			for(t = 1; t < cycle+1; t++) //��1ʱ�̵�cycle
			{
				IloNum temp1 = IloMin(cycle, t+thminUp[i]-1);//the minimum of an array of numeric expressions or over a numeric expression    and a constant in C++
				IloNum temp2 = IloMin(cycle, t+thminDown[i]-1);
				IloExpr sum1(env);
				IloExpr sum2(env);
				for(k = t+1; k <= temp1; k++)
				{
					sum1 += shutDown[i][k];
				}
				for(k = t+1; k <= temp2; k++)
				{
					sum2 += startUp[i][k];
				}
				model.add(startUp[i][t] + sum1 <= 1);
				model.add(shutDown[i][t] + sum2 <= 1);
			}
		}

		//��ʼ���ػ�ʱ��Լ��
		for(i=0;i<thUnitNum;i++)
		{
			if(thinitState[i]<0&&IloAbs(thinitState[i])<thminDown[i])
			{
				for(t=1;t<=(thminDown[i]+thinitState[i]);t++)
				{
					model.add(startUp[i][t]==0);
					model.add(state[i][t]==0);
				}
			}
			if(thinitState[i]>0&&thinitState[i]<thminUp[i])
			{
				for(t=1;t<=(thminUp[i]-thinitState[i]);t++)
				{
					model.add(shutDown[i][t] == 0);
					model.add(state[i][t] == 1);
				}
			}
		}
		//���ػ�����Լ������1ʱ�̿�ʼ��
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{
				IloNum temp=IloMin(cycle,(t+thminUp[i]+thminDown[i]-1));
				IloExpr sum1(env);
				IloExpr sum2(env);
				for(j=t; j<=temp; j++)
				{
					sum1+=startUp[i][j];
				}
				for(j=t;j<=temp;j++)
				{
					sum2 += shutDown[i][j];
				}
				model.add(sum1-1 <= 0);
				model.add(sum2-1 <= 0);
			}
		}

		//the constraint of fuel cost
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{
				model.add (fuelCost[i][t] == thb[i]*thermalPower[i][t] + thc[i]);
			}
		}

		//��������Լ��	  
		for(i=0;i<thUnitNum;i++)
		{
			for(t=1;t<cycle+1;t++)
			{
				model.add(upCost[i][t]<=thcoldUpCost[i]*startUp[i][t]);
				model.add(upCost[i][t]>=thhotUpCost[i]*startUp[i][t]);
			}
		}

		//��������ĩ����

		for (i=0;i<thUnitNum; i++)
		{
			for (t=1; t<cycle+1;t++)
			{
				model.add(IloAbs(thermalPower[i][t] - thermalPower[i][t-1]) <= thdelta[i]);
			}

		}

		//**********************************************Լ��**************************************************
		//����ƽ��Լ��
		for( t=1;t<cycle+1;t++)
		{
			IloExpr demandsum(env);
			IloExpr thsum(env);			//���
			IloExpr hydsum(env);		//ˮ��
			IloExpr windsum(env);		//���
			IloExpr solarsum(env);		//���
			IloExpr outputsum(env);     //����
			IloExpr secsum(env);		//����
			for(i=0;i<thUnitNum;i++)
			{
				thsum += thermalPower[i][t];
			}

			for(k=0;k<hyUnitNum;k++)
			{
				hydsum += hyPower[k][t];
			}

			for(w=0;w<wFieldNum;w++)
			{
				windsum += windPower[w][t];
			}

			for(s=0;s<sPlantNum;s++)
			{
				solarsum += maxSPlantPower[s][t];
			}
			for(d=0; d<demandNum; ++d)
			{
				demandsum += Demand[d][t];
			}
			for(i=0;i<outputNum;i++)
			{
				outputsum += outputPower[i][t];
			}
			model.add(demandsum  == thsum + hydsum + windsum + solarsum + outputsum );
		}

		/************************************************************************/
		/*					Spinning reserve	       		By Chun-Ting	    */
		/************************************************************************/
		//VarMatrix2 outputPower(env,outputNum);
		VarMatrix2 windR1(env,wFieldNum);
		VarMatrix2 hydroR1(env,hyUnitNum);
		VarMatrix2 windR2(env,wFieldNum);
		VarMatrix2 hydroR2(env,hyUnitNum);
		for(w=0; w<wFieldNum; ++w)
			windR1[w] = IloNumVarArray(env,cycle+1,0,maxWindPower[w],ILOFLOAT);

		for(k=0; k<hyUnitNum; ++k)
			hydroR1[k] = IloNumVarArray(env,cycle+1,0 ,maxHyUnitPower[k],ILOFLOAT);

		for(w=0; w<wFieldNum; ++w)
			windR2[w] = IloNumVarArray(env,cycle+1,0,maxWindPower[w],ILOFLOAT);

		for(k=0; k<hyUnitNum; ++k)
			hydroR2[k] = IloNumVarArray(env,cycle+1,0, maxHyUnitPower[k],ILOFLOAT);

		for(t=1;t<cycle+1;t++)
		{
			IloExpr thsum(env);
			IloExpr thsumN(env);
			IloExpr hysum(env);  //ˮ�������ĳʱ���ṩ��������
			IloExpr hysumN(env); //ˮ�������ĳʱ���ṩ�ĸ�����
			IloExpr windRP(env);
			IloExpr windRN(env);
			//IloExpr outputRP(env);
			//IloExpr outputRN(env);
			for(i=0;i<thUnitNum;i++)
			{
				model.add(thermalR[i][t] <= IloMin(thmaxPower[i]*state[i][t] - thermalPower[i][t], thmaxR[i]*state[i][t]));
				model.add(thermalRN[i][t] == IloMin(thermalPower[i][t] - thminPower[i]*state[i][t], thmaxR[i]*state[i][t]));
				thsum += thermalR[i][t];
				thsumN += thermalRN[i][t];
			}

			for(w=0; w<wFieldNum;++w)
			{
				model.add(windR1[w][t] == IloMin(r*windPower[w][t], maxWindPower[w]-windPower[w][t]));
				model.add(windR2[w][t] == IloMin(r*windPower[w][t], windPower[w][t]-minWindPower[w]));
				windRP += windR2[w][t];
				windRN += windR1[w][t];
			}

			for(k=0;k<hyUnitNum;k++)
			{
        if( hyPower[k][t] < 0 ) hyPower[k][t] = 0;
        if( hyPower[k][t] > maxHyUnitPower[k] ) hyPower[k][t] = maxHyUnitPower[k];
				model.add(hydroR1[k][t] == IloAbs( maxHyUnitPower[k] - hyPower[k][t] ));
				model.add(hydroR2[k][t] == IloAbs( hyPower[k][t] - minHyUnitPower[k] ));
				hysum += hydroR1[k][t];
				hysumN += hydroR2[k][t];
			}
			model.add(thsum + hysum  >= sysReserve[t] + windRP);  //������Լ��
			model.add(thsumN + hysumN >= sysReserve[t] +  windRN);  //������Լ��
		}

			/**************************************************************/
			//�����߰�ȫԼ��
			for(t=1;t<cycle+1;t++)
			{
				for(l=0;l<lineNum;l++)
				{
					IloExpr thsum(env);			//���
					IloExpr hydsum(env);		//ˮ��
					IloExpr windsum(env);		//���
					IloExpr solarsum(env);		//���
					IloExpr demandsum(env);		//����
					IloExpr sumGamaO(env);
					IloExpr secsum(env);		//����
					for(i=0;i<thUnitNum;i++)
					{
						thsum += gama[l][unitLocation[i]-1]*thermalPower[i][t];
					}

					for(k=0; k<hyUnitNum; k++)
					{
						hydsum += gama[l][hyUnitLocation[k]-1]*hyPower[k][t];
					}

					for(w=0; w<wFieldNum; w++)
					{
						windsum += gama[l][windLocation[w]-1]*windPower[w][t];
					}

					for(s=0;s<sPlantNum;s++)
					{
						solarsum += gama[l][sPlantLocation[s]-1]*maxSPlantPower[s][t];
					}

					for(d=0;d<demandNum;d++)
					{
						demandsum += gama[l][demandLocation[d]-1]*Demand[d][t];
					}

					for (o= 0; o < outputNum; ++o)
					{
						sumGamaO += gama[l][outputLocation[o]-1]*outputPower[o][t];
					}
					model.add(thsum + hydsum + windsum + solarsum + sumGamaO - demandsum + 4*lineCap[l] >= 0);
          // The constraint below doesn't work.
					model.add(thsum + hydsum + windsum + solarsum +  sumGamaO - demandsum - 4*lineCap[l] <= 0 );
				}
			}

/*			//����Լ��
			for(t=0;t<cycle;t++)
			{
				for(int i_sec=0;i_sec<sectionNum;i_sec++)
				{
					IloExpr thsum(env);			//���
					IloExpr hydsum(env);		//ˮ��
					IloExpr windsum(env);		//���
					IloExpr solarsum(env);		//���
					IloExpr demandsum(env);		//����
					IloExpr secsum(env);		//����
					int i_linsec=0;
					l=linSection[i_sec][0];
					while(l<=linSectionNum[i_sec]) {
						for(i=0;i<thUnitNum;i++)
						{
							thsum+=gama[l][unitLocation[i]-1]*thermalPower[i][t];
						}

						for(k=0;k<hyUnitNum;k++)
						{
							hydsum+=gama[l][hyUnitLocation[k]-1]*hyPower[k][t];
						}

						for(w=0;w<wFieldNum;w++)
						{
							windsum+=gama[l][windLocation[w]-1]*windPower[w][t];
						}

						for(s=0;s<sPlantNum;s++)
						{
							solarsum+=gama[l][sPlantLocation[s]-1]*maxSPlantPower[s][t];
						}

						for(d=0;d<demandNum;d++)
						{
							demandsum+=gama[l][demandLocation[d]-1]*Demand[d][t+1];
						}

						secsum+=thsum+hydsum+windsum+solarsum-demandsum;

						i_linsec++;
						l=linSection[i_sec][i_linsec];
					}
					model.add(secsum>=downSectionCap[i_sec]);
					model.add(secsum<=upSectionCap[i_sec]);
				}
			}
*/
			//�����Ż�Ŀ�꺯��
			IloExpr obj(env);
			for(t=1;t<cycle+1;t++)	{
				for(i=0;i<thUnitNum;i++) {
					obj+=fuelCost[i][t]+upCost[i][t];
				}

			}

			IloObjective objective = IloMinimize(env, obj);
			model.add(objective);

			IloCplex cplex(model);
			cplex.setParam(cplex.EpGap,0.001);//relative MIP gap tolerance
			//	cplex.setParam(cplex.NodeFileInd,3);
			//	cplex.setParam(cplex.TiLim,100);

			cplex.extract(model);
			cplex.solve();
			cplex.exportModel(lp);//��IloCplex.importModel��Ӧ�����Զ���ģ��

			env.out() << "Solution status = " << cplex.getStatus() << endl;
			env.out() << "Solution value  = " << cplex.getObjValue() << endl;
			env.out() << "solution time   = " <<timer.getTime()<<endl;
			env.out() << "EpGap           = " <<cplex.getParam(cplex.EpGap)<<endl;

			ofstream outf(OUTFILERESULT,ios::out);
			if(!outf)
				cout<<"cannot open 'result.dat'"<<OUTFILERESULT<<endl;

			outf<<"Result"<<endl;
			outf<<"Solution status\t"<<cplex.getStatus()<<endl;
			outf<<"Solution value\t"<<cplex.getObjValue()<<endl;
			outf<<"Solution time\t"<<timer.getTime()<<endl;
			outf<<"EpGap\t"<<cplex.getParam(cplex.EpGap)<<endl;
			double allFuelCost=0;
			for(i=0;i<thUnitNum;i++)
			{
				for(t=1;t<cycle+1;t++)
				{
					allFuelCost+=cplex.getValue(fuelCost[i][t]);
				}
			}
			cout<<endl<<"allFuelCost = "<<allFuelCost<<endl;
			outf<<"allFuelCost\t"<<allFuelCost<<endl;
			if(!outf)
				cout<<"cannot open 'Result.dat'"<<OUTFILERESULT<<endl;
			outf<<"state"<<endl;
			for(t=1;t<cycle+1;t++)
			{
				for(i=0;i<thUnitNum;i++)
				{
					if(cplex.getValue(state[i][t])<_INF)
						outf<<"0\t";
					else
						outf<<cplex.getValue(state[i][t])<<"\t";
				}
				outf<<endl;
			}

			outf<<endl<<"thermalPower"<<endl;
			for(i=0;i<thUnitNum;i++)
			{
				for(t=1;t<cycle+1;t++)
				{
					if(cplex.getValue(thermalPower[i][t])<_INF)
						outf<<"0\t";
					else
						outf<<cplex.getValue(thermalPower[i][t])<<"\t";
				}
				outf<<endl;
			}

			outf<<endl<<"thermalR"<<endl;
			for(i=0;i<thUnitNum;i++)
			{
				for(t=1;t<cycle+1;t++)
				{
					if(cplex.getValue(thermalR[i][t])<1e-7)
						outf<<"0\t";
					else
						outf<<cplex.getValue(thermalR[i][t])<<"\t";
				}
				outf<<endl;
			}

			outf<<"current[l][t]"<<endl;
			double current[655][97];
			for(l= 0; l< lineNum; l++) {
			for (t = 1; t < cycle+1; t++)	{
					double gamap=0;
					for (i = 0; i < thUnitNum; i++)
					{
						gamap+=gama[l][unitLocation[i]-1]*cplex.getValue(thermalPower[i][t]);
					}
					double gamaD=0;
					for (d = 0; d < demandNum; d++)
					{
						gamaD+=gama[l][demandLocation[d]-1]*Demand[d][t];
					}
          current[l][t]=gamap-gamaD;
					outf<<current[l][t]<<"\t";

				}
				outf<<endl;
			}

			outf<<endl;

		}
		catch (IloException& e)
		{
			cerr << "Concert exception caught: " << e << endl;
		}

		catch (...)
		{
			cerr << "Unknown exception caught" << endl;
		}

		env.end();
	//	system("pause");
		return 0;
	}
