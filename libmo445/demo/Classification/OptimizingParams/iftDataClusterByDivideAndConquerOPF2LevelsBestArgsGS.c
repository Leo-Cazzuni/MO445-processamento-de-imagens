#include <ift.h>

double find_best_args(iftDict* problem, iftDict* params) {

  int limit_nb_groups = iftGetLongValFromDict("limit_nb_groups", problem);
  int exec_times=iftGetLongValFromDict("exec_times", problem);

  iftDataSet *Z;
  timer *t1=NULL,*t2=NULL;
  float measure_acc=0;
  float time_acc=0;
  int nb_groups_acc=0;
  bool exited=false;
  float *acc_vetor=iftAllocFloatArray(exec_times);

  int nb_partitions=(int)iftGetDblValFromDict("nb_part_arg", params);
  float kmax1_arg=iftGetDblValFromDict("kmax1_arg",params);
  float kmax2_arg=iftGetDblValFromDict("kmax2_arg",params);

  Z = iftReadOPFDataSet(iftGetStrValFromDict("dataset_path",problem));

  for (int i=0;i<exec_times && !exited;i++){
    iftSetStatus(Z,IFT_TRAIN);

    t1 = iftTic();
    iftDivideAndConquerUnsupOPF2Levels(Z, nb_partitions,0,iftNormalizedCut, kmax1_arg, kmax2_arg, 1);
    t2     = iftToc();

    if (Z->ngroups > 2*limit_nb_groups){
      measure_acc=IFT_INFINITY_FLT_NEG;
      exited=true;
    }
    else{
      nb_groups_acc+=Z->ngroups;
      time_acc+=iftCompTime(t1,t2);
      iftPropagateClusterTrueLabels2(Z);
      iftSetStatus(Z,IFT_TEST);
      acc_vetor[i]=iftTruePositives(Z);
      measure_acc+=acc_vetor[i];
    }
  }

  if (!exited && (nb_groups_acc/exec_times <= limit_nb_groups)){
    time_acc/=exec_times;
    measure_acc/=exec_times;
  }
  else
    measure_acc=IFT_INFINITY_FLT_NEG;

  printf("\nWith kmax1 = %.5f and kmax2 = %.5f and time = %.3f and nb_groups = %d ->acc = %.4f and std = %.4f\n",kmax1_arg,kmax2_arg,time_acc,nb_groups_acc/exec_times,measure_acc,iftStddevFloatArray(acc_vetor,exec_times));
  fflush(stdout);

  iftDestroyDataSet(&Z);
  iftFree(acc_vetor);

  return measure_acc;

}

int main(int argc, char *argv[])
{

  iftRandomSeed(time(NULL));

  if (argc != 13)
    iftError("Usage: iftDataClusterByDivideAndConquerOPF2LevelsBestArgsGS <dataset.zip> <limit_nb_groups> <init_nb_part> <final_nb_part> <step_nb_part> <init_kmax1> <final_kmax1> <step_kmax1> <init_kmax2> <final_kmax2> <step_kmax2> <exec_times>","main");

  iftDict* params = iftCreateDict();
  iftDict* problem = iftCreateDict();

  iftInsertIntoDict("nb_part_arg", iftRange(atof(argv[3]), atof(argv[4]), atof(argv[5])),params);
  iftInsertIntoDict("kmax1_arg", iftRange(atof(argv[6]), atof(argv[7]), atof(argv[8])),params);
  iftInsertIntoDict("kmax2_arg", iftRange(atof(argv[9]),atof(argv[10]), atof(argv[11])),params);

  iftInsertIntoDict("dataset_path", argv[1],problem);
  iftInsertIntoDict("limit_nb_groups", atoi(argv[2]),problem);
  iftInsertIntoDict("exec_times", atoi(argv[12]),problem);

  iftDict *result =iftGridSearch(params, find_best_args, problem);

  printf("Best parameters for %s dataset\n",argv[1]);

  printf("best_num_partitions_arg -> %d\n",(int)iftGetDblValFromDict("nb_part_arg",result));
  printf("best_kmax1_arg -> %.4f\n",(float)iftGetDblValFromDict("kmax1_arg",result));
  printf("best_kmax2_arg -> %.4f\n",(float)iftGetDblValFromDict("kmax2_arg",result));
  printf("best_objetive_func -> %.4f\n",(float)iftGetDblValFromDict("best_func_val",result));

  iftDestroyDict(&params);
  iftDestroyDict(&problem);
  iftDestroyDict(&result);

  return(0);
}
