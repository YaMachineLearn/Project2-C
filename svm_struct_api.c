/***********************************************************************/
/*                                                                     */
/*   svm_struct_api.c                                                  */
/*                                                                     */
/*   Definition of API for attaching implementing SVM learning of      */
/*   structures (e.g. parsing, multi-label classification, HMM)        */ 
/*                                                                     */
/*   Author: Thorsten Joachims                                         */
/*   Date: 03.07.04                                                    */
/*                                                                     */
/*   Copyright (c) 2004  Thorsten Joachims - All rights reserved       */
/*                                                                     */
/*   This software is available for non-commercial use only. It must   */
/*   not be modified and distributed without prior permission of the   */
/*   author. The author is not responsible for implications from the   */
/*   use of this software.                                             */
/*                                                                     */
/***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "svm_struct/svm_struct_common.h"
#include "svm_struct_api.h"

void svm_struct_learn_api_init(int argc, char* argv[])
{
  /* Called in learning part before anything else is done to allow
     any initializations that might be necessary. */
}

void svm_struct_learn_api_exit()
{
  /* Called in learning part at the very end to allow any clean-up
     that might be necessary. */
}

void svm_struct_classify_api_init(int argc, char* argv[])
{
  /* Called in prediction part before anything else is done to allow
     any initializations that might be necessary. */
}

void svm_struct_classify_api_exit()
{
  /* Called in prediction part at the very end to allow any clean-up
     that might be necessary. */
}

SAMPLE read_struct_examples(char *file, STRUCT_LEARN_PARM *sparm)
{
  /* Reads struct examples and returns them in sample. The number of
     examples must be written into sample.n */
  SAMPLE   sample;  /* sample */
  EXAMPLE  *examples;
  int      *utterFrameCount;
  long     n;       /* number of utterance */

  n = 0;
  int featDim = 69;

  /* parse filenames */
  char *file1 = strtok(file, ",");
  char *file2 = strtok(NULL, ",");

  /* utterance count  */
  FILE *fpUtter;

  fpUtter = fopen(file1, "r");
  int utterIndex = 0;
  int tempValueUtter = 0;

  fscanf(fpUtter, "%d", &tempValueUtter);
  n = tempValueUtter;

  utterFrameCount = (int *)my_malloc(n * sizeof(int));
  examples = (EXAMPLE *)my_malloc(n * sizeof(EXAMPLE));

  while (fscanf(fpUtter, "%d", &tempValueUtter) != EOF) {
    utterFrameCount[utterIndex] = tempValueUtter;
    utterIndex++;
  }

  fclose(fpUtter);

  /* initialize examples */
  int i = 0;
  for (i = 0; i < n; ++i) {
    examples[i].x.length = utterFrameCount[i];
    examples[i].x.features = (double **)my_malloc(utterFrameCount[i] * sizeof(double *));
    examples[i].y.length = utterFrameCount[i];
    examples[i].y.labels = (int *)my_malloc(utterFrameCount[i] * sizeof(int));
  }

  /* read training data */
  FILE *dataFile;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  dataFile = fopen(file2, "r");

  int lineIndex = 1;
  int label = 0;
  utterIndex = 0;
  int frameIndex = 0;
  char *pch;
  int dimIndex = 0;
  double tempValue = 0;

  while ((read = getline(&line, &len, dataFile)) != -1) {
    if (lineIndex % 2 == 1) {
      sscanf(line, "%d", &label);
      examples[utterIndex].y.labels[frameIndex] = label;
    }
    else {
      examples[utterIndex].x.features[frameIndex] = (double *)my_malloc(featDim * sizeof(double));
      pch = strtok(line, " \n");
      dimIndex = 0;
      while (pch != NULL)
      {
        // printf("%s\n", pch);
        sscanf(pch, "%lf", &tempValue);
        examples[utterIndex].x.features[frameIndex][dimIndex] = tempValue;
        dimIndex++;
        pch = strtok(NULL, " \n");
      }
      frameIndex++;
    }
    lineIndex++;

    if (frameIndex == utterFrameCount[utterIndex]) {
      utterIndex++;
      frameIndex = 0;
    }
  }

  sample.n = n;
  sample.examples = examples;
  return sample;
}

void init_struct_model(SAMPLE sample, STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm, LEARN_PARM *lparm, KERNEL_PARM *kparm)
{
  /* Initialize structmodel sm. The weight vector w does not need to be
     initialized, but you need to provide the maximum size of the
     feature space in sizePsi. This is the maximum number of different
     weights that can be learned. Later, the weight vector w will
     contain the learned weights for the model. */
  sparm->num_features = 69;
  sparm->num_classes = 48;
  sm->sizePsi = sparm->num_features * sparm->num_classes + sparm->num_classes * sparm->num_classes; /* replace by appropriate number of features */
}

CONSTSET init_struct_constraints(SAMPLE sample, STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm)
{
  /* Initializes the optimization problem. Typically, you do not need
     to change this function, since you want to start with an empty
     set of constraints. However, if for example you have constraints
     that certain weights need to be positive, you might put that in
     here. The constraints are represented as lhs[i]*w >= rhs[i]. lhs
     is an array of feature vectors, rhs is an array of doubles. m is
     the number of constraints. The function returns the initial
     set of constraints. */
  CONSTSET c;
  long     sizePsi=sm->sizePsi;
  long     i;
  WORD     words[2];

  if (1) { /* normal case: start with empty set of constraints */
    c.lhs=NULL;
    c.rhs=NULL;
    c.m=0;
  }
  else { /* add constraints so that all learned weights are
            positive. WARNING: Currently, they are positive only up to
            precision epsilon set by -e. */
    c.lhs=my_malloc(sizeof(DOC *)*sizePsi);
    c.rhs=my_malloc(sizeof(double)*sizePsi);
    for (i=0; i<sizePsi; i++) {
      words[0].wnum=i+1;
      words[0].weight=1.0;
      words[1].wnum=0;
      /* the following slackid is a hack. we will run into problems,
         if we have move than 1000000 slack sets (ie examples) */
      c.lhs[i]=create_example(i,0,1000000+i,1,create_svector(words,"",1.0));
      c.rhs[i]=0.0;
    }
  }
  return c;
}

LABEL classify_struct_example(PATTERN x, STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label yhat for pattern x that scores the highest
     according to the linear evaluation function in sm, especially the
     weights sm.w. The returned label is taken as the prediction of sm
     for the pattern x. The weights correspond to the features defined
     by psi() and range from index 1 to index sm->sizePsi. If the
     function cannot find a label, it shall return an empty label as
     recognized by the function empty_label(y). */
  LABEL y;

  /* insert your code for computing the predicted label y here */
  // use 'w' just for testing
  // 'w' should be replaced with sm.w later
  double w[21] = {0.0, 2.0, 1.0, 6.0, 3.0, 7.0, 2.0, 1.0, 5.0, 3.0, 4.0, 4.0, 2.0, 2.0, 4.0, 3.0, 2.0, 2.0, 4.0, 3.0, 1.0, 1.0};
  int lab, lastLab, frameIndex, i, maxCostIndex;
  int obsFeatDim = sparm->num_features * sparm->num_classes;
  double sum, maxCost, temp;

  int **lastPhone = (int **)my_malloc(sparm->num_classes * sizeof(int *));
  double **cost = (double **)my_malloc(sparm->num_classes * sizeof(double *));
  for (lab = 0; lab < sparm->num_classes; lab++) {
    lastPhone[lab] = (int *)my_malloc(x.length * sizeof(int));
    cost[lab] = (double *)my_malloc(x.length * sizeof(double));
  }

  for (lab = 0; lab < sparm->num_classes; lab++) {
    cost[lab][0] = 0.0;
    for (i = 0; i < sparm->num_features; i++) {
      cost[lab][0] += x.features[0][i] * sm->w[1 + sparm->num_features * lab + i];
    }    
  }

  for (frameIndex = 1; frameIndex < x.length; frameIndex++) {
    for (lab = 0; lab < sparm->num_classes; lab++) {
      maxCostIndex = 0;
      sum = 0.0;
      for (i = 0; i < sparm->num_features; i++) {
        sum += x.features[frameIndex][i] * sm->w[1 + sparm->num_features * lab + i];
      }
      maxCost = cost[0][frameIndex - 1] + sm->w[1 + obsFeatDim + lab] + sum;
      for (lastLab = 1; lastLab < sparm->num_classes; lastLab++) {
        temp = cost[lastLab][frameIndex - 1] + sm->w[1 + obsFeatDim + lastLab * sparm->num_classes + lab] + sum;
        if (temp > maxCost) {
          maxCostIndex = lastLab;
          maxCost = temp;
        }
      }
      lastPhone[lab][frameIndex] = maxCostIndex;
      cost[lab][frameIndex] = maxCost;
    }
  }

  maxCostIndex = 0;
  maxCost = cost[0][x.length - 1];
  for (lab = 1; lab < sparm->num_classes; lab++) {
    temp = cost[lab][x.length - 1];
    if (temp > maxCost) {
      maxCostIndex = lab;
      maxCost = temp;
    }
  }
  y.length = x.length;
  y.labels = (int *)my_malloc(y.length * sizeof(int));
  y.labels[y.length - 1] = maxCostIndex;
  for (frameIndex = y.length - 2; frameIndex >= 0; frameIndex--) {
    maxCostIndex = lastPhone[maxCostIndex][frameIndex + 1];
    y.labels[frameIndex] = maxCostIndex;
  }

  for (lab = 0; lab < sparm->num_classes; lab++) {
    free(lastPhone[lab]);
    free(cost[lab]);
  }
  free(lastPhone);
  free(cost);
  return(y);
}

LABEL find_most_violated_constraint_slackrescaling(PATTERN x, LABEL y, STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the slack rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)*(1-psi(x,y)+psi(x,ybar)) 

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
  LABEL ybar;

  /* insert your code for computing the label ybar here */
  
  return(ybar);
}

LABEL find_most_violated_constraint_marginrescaling(PATTERN x, LABEL y, STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the margin rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)+psi(x,ybar)

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
  LABEL ybar;

  /* insert your code for computing the label ybar here */
  // use 'w' just for testing
  // 'w' should be replaced with sm.w later
  double w[21] = {0.0, 2.0, 1.0, 6.0, 3.0, 7.0, 2.0, 1.0, 5.0, 3.0, 4.0, 4.0, 2.0, 2.0, 4.0, 3.0, 2.0, 2.0, 4.0, 3.0, 1.0, 1.0};
  int lab, lastLab, frameIndex, i, maxCostIndex;
  int obsFeatDim = sparm->num_features * sparm->num_classes;
  double sum, maxCost, temp;

  int **lastPhone = (int **)my_malloc(sparm->num_classes * sizeof(int *));
  double **cost = (double **)my_malloc(sparm->num_classes * sizeof(double *));
  for (lab = 0; lab < sparm->num_classes; lab++) {
    lastPhone[lab] = (int *)my_malloc(x.length * sizeof(int));
    cost[lab] = (double *)my_malloc(x.length * sizeof(double));
  }

  for (lab = 0; lab < sparm->num_classes; lab++) {
    cost[lab][0] = 0.0;
    for (i = 0; i < sparm->num_features; i++) {
      cost[lab][0] += x.features[0][i] * sm->w[1 + sparm->num_features * lab + i];
    }
    if (lab != y.labels[0])
      cost[lab][0] += 1.0;
  }

  for (frameIndex = 1; frameIndex < x.length; frameIndex++) {
    for (lab = 0; lab < sparm->num_classes; lab++) {
      maxCostIndex = 0;
      sum = 0.0;
      for (i = 0; i < sparm->num_features; i++) {
        sum += x.features[frameIndex][i] * sm->w[1 + sparm->num_features * lab + i];
      }
      maxCost = cost[0][frameIndex - 1] + sm->w[1 + obsFeatDim + lab] + sum;
      for (lastLab = 1; lastLab < sparm->num_classes; lastLab++) {
        temp = cost[lastLab][frameIndex - 1] + sm->w[1 + obsFeatDim + lastLab * sparm->num_classes + lab] + sum;
        if (temp > maxCost) {
          maxCostIndex = lastLab;
          maxCost = temp;
        }
      }
      if (lab != y.labels[frameIndex])
        maxCost += 1.0;
      lastPhone[lab][frameIndex] = maxCostIndex;
      cost[lab][frameIndex] = maxCost;
    }
  }

  maxCostIndex = 0;
  maxCost = cost[0][x.length - 1];
  for (lab = 1; lab < sparm->num_classes; lab++) {
    temp = cost[lab][x.length - 1];
    if (temp > maxCost) {
      maxCostIndex = lab;
      maxCost = temp;
    }
  }
  // printf("my_maxCost: %f\n", maxCost);
  ybar.length = x.length;
  ybar.labels = (int *)my_malloc(ybar.length * sizeof(int));
  ybar.labels[ybar.length - 1] = maxCostIndex;
  for (frameIndex = ybar.length - 2; frameIndex >= 0; frameIndex--) {
    maxCostIndex = lastPhone[maxCostIndex][frameIndex + 1];
    ybar.labels[frameIndex] = maxCostIndex;
  }

  // SVECTOR *svec = psi(x, ybar, sm, sparm);
  // printf("the_maxCost %f\n", sprod_ns(sm->w, svec) + loss(y, ybar, sparm));
  // double maxCost2 = 0.0;
  // for (i = 0; i < sm->sizePsi + 1; i++) {
  //   maxCost2 += sm->w[svec->words[i].wnum] * svec->words[i].weight;
  //   if (svec->words[i].weight > 1000)
  //     printf("weight is %f when i = %d\n", svec->words[i].weight, i);
  //   if (svec->words[i].wnum == 0)
  //     printf("wnum is 0 when i = %d\n", i);
  // }
  // maxCost2 += + loss(y, ybar, sparm);
  // printf("maxCost2: %f\n", maxCost2);

  // int lab1, lab2, lab3, lab4, lab5;
  // int maxIndex[5];
  // LABEL ybar2;
  // ybar2.length = y.length;
  // ybar2.labels = (int *)my_malloc(y.length * sizeof(int));
  // ybar2.labels[0] = 0;
  // ybar2.labels[1] = 0;
  // ybar2.labels[2] = 0;
  // ybar2.labels[3] = 0;
  // ybar2.labels[4] = 0;
  // SVECTOR *svec2 = psi(x, ybar2, sm, sparm);
  // maxCost = sprod_ns(sm->w, svec2) + loss(y, ybar2, sparm);
  // maxIndex[0] = 0;
  // maxIndex[1] = 0;
  // maxIndex[2] = 0;
  // maxIndex[3] = 0;
  // maxIndex[4] = 0;
  // for (lab1 = 0; lab1 < sparm->num_classes; lab1++) {
  //   for (lab2 = 0; lab2 < sparm->num_classes; lab2++) {
  //     for (lab3 = 0; lab3 < sparm->num_classes; lab3++) {
  //       for (lab4 = 0; lab4 < sparm->num_classes; lab4++) {
  //         for (lab5 = 0; lab5 < sparm->num_classes; lab5++) {
  //           ybar2.labels[0] = lab1;
  //           ybar2.labels[1] = lab2;
  //           ybar2.labels[2] = lab3;
  //           ybar2.labels[3] = lab4;
  //           ybar2.labels[4] = lab5;
  //           svec2 = psi(x, ybar2, sm, sparm);
  //           temp = sprod_ns(sm->w, svec2) + loss(y, ybar2, sparm);
  //           // if (lab1 == 1 && lab2 == 1 && lab3 == 2)
  //           //   printf("temp = %f, maxCost = %f\n", temp, maxCost);
  //           if (temp > maxCost) {
  //             // if (lab1 == 1 && lab2 == 1 && lab3 == 2)
  //             //   printf("Go in!\n");
  //             maxCost = temp;
  //             maxIndex[0] = lab1;
  //             maxIndex[1] = lab2;
  //             maxIndex[2] = lab3;
  //             maxIndex[3] = lab4;
  //             maxIndex[4] = lab5;
  //           }
  //         }
  //       }
  //     }
  //   }
  // }
  // printf("maxCost = %f, maxIndex = [%d][%d][%d][%d][%d]\n", maxCost, maxIndex[0], maxIndex[1], maxIndex[2], maxIndex[3], maxIndex[4]);

  // printf("cost:\n");
  // for (lab = 0; lab < sparm->num_classes; lab++) {
  //   for (frameIndex = 0; frameIndex < x.length; frameIndex++) {
  //     printf("%f ", cost[lab][frameIndex]);
  //   }
  //   printf("\n");
  // }
  // printf("lastPhone:\n");
  // for (lab = 0; lab < sparm->num_classes; lab++) {
  //   for (frameIndex = 0; frameIndex < x.length; frameIndex++) {
  //     printf("%d ", lastPhone[lab][frameIndex]);
  //   }
  //   printf("\n");
  // }
  // printf("ybar:\n");
  // for (frameIndex = 0; frameIndex < ybar.length; frameIndex++) {
  //   printf("%d ", ybar.labels[frameIndex]);
  // }
  // printf("\n");

  // printf("\nIn find_most_violated_constraint:\nsm.w\n");
  // for (i = 0; i < sm->sizePsi + 1; i++) {
  //   if (sm->w[i] != 0.0)
  //     printf("%d:%f ", i, sm->w[i]);
  // }

  for (lab = 0; lab < sparm->num_classes; lab++) {
    free(lastPhone[lab]);
    free(cost[lab]);
  }
  free(lastPhone);
  free(cost);
  return(ybar);
}

int empty_label(LABEL y)
{
  /* Returns true, if y is an empty label. An empty label might be
     returned by find_most_violated_constraint_???(x, y, sm) if there
     is no incorrect label that can be found for x, or if it is unable
     to label x at all */
  if (y.length == 0)
    return 1;
  return(0);
}

SVECTOR *psi(PATTERN x, LABEL y, STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm)
{
  /* Returns a feature vector describing the match between pattern x
     and label y. The feature vector is returned as a list of
     SVECTOR's. Each SVECTOR is in a sparse representation of pairs
     <featurenumber:featurevalue>, where the last pair has
     featurenumber 0 as a terminator. Featurenumbers start with 1 and
     end with sizePsi. Featuresnumbers that are not specified default
     to value 0. As mentioned before, psi() actually returns a list of
     SVECTOR's. Each SVECTOR has a field 'factor' and 'next'. 'next'
     specifies the next element in the list, terminated by a NULL
     pointer. The list can be though of as a linear combination of
     vectors, where each vector is weighted by its 'factor'. This
     linear combination of feature vectors is multiplied with the
     learned (kernelized) weight vector to score label y for pattern
     x. Without kernels, there will be one weight in sm.w for each
     feature. Note that psi has to match
     find_most_violated_constraint_???(x, y, sm) and vice versa. In
     particular, find_most_violated_constraint_???(x, y, sm) finds
     that ybar!=y that maximizes psi(x,ybar,sm)*sm.w (where * is the
     inner vector product) and the appropriate function of the
     loss + margin/slack rescaling method. See that paper for details. */
  int featNum = 69;
  int labelNum = 48;
  double *psiVec = (double *)my_malloc((featNum + labelNum) * labelNum * sizeof(double));

  int i, l ,j;
  for (i = 0; i < (featNum + labelNum) * labelNum; i++)
    psiVec[i] = 0.0;
  for (l = 0; l < x.length; ++l)
    for (j = 0; j < featNum; ++j)
      psiVec[featNum * y.labels[l] + j] += x.features[l][j];

  int k = 0;
  int ybegin = featNum * labelNum;
  for (k = 0; k < (y.length - 1); ++k)
    psiVec[ybegin + (labelNum * y.labels[k]) + y.labels[k + 1]] += 1;

  SVECTOR *fvec = (SVECTOR *)my_malloc(sizeof(SVECTOR));

  /* insert code for computing the feature vector for x and y here */
  // double psiVec[21] = {2.0, 1.0, 6.0, 3.0, 7.0, 2.0, 1.0, 5.0, 3.0, 4.0, 4.0, 2.0, 2.0, 4.0, 3.0, 2.0, 2.0, 4.0, 3.0, 1.0, 1.0};

  fvec->words = (WORD *)my_malloc((sm->sizePsi + 1) * sizeof(WORD));
  for (i = 0; i < sm->sizePsi; i++) {
    fvec->words[i].wnum = i + 1;
    fvec->words[i].weight = psiVec[i];
  }
  fvec->words[sm->sizePsi].wnum = 0;
  fvec->twonorm_sq = -1;
  fvec->userdefined = NULL;
  fvec->kernel_id = 0;
  fvec->next = NULL;
  fvec->factor = 1;

  free(psiVec);

  return fvec;
}

double loss(LABEL y, LABEL ybar, STRUCT_LEARN_PARM *sparm)
{
  /* loss for correct label y and predicted label ybar. The loss for
     y==ybar has to be zero. sparm->loss_function is set with the -l option. */
  if(sparm->loss_function == 0) { /* type 0 loss: 0/1 loss */
                                  /* return 0, if y==ybar. return 1 else */
    if (y.length != ybar.length) {
      printf("Error: y and ybar length mismatch!");
      exit(1);
    }
    int i, num = 0;
    for (i = 0; i < y.length; i++) {
      if (y.labels[i] != ybar.labels[i])
        num++;
    }
    return (double)num;
  }
  else {
    /* Put your code for different loss functions here. But then
       find_most_violated_constraint_???(x, y, sm) has to return the
       highest scoring label with the largest loss. */
    printf("Error: Unknown type of loss function!");
    exit(1);
  }
}

int finalize_iteration(double ceps, int cached_constraint, SAMPLE sample, STRUCTMODEL *sm, CONSTSET cset, double *alpha, STRUCT_LEARN_PARM *sparm)
{
  /* This function is called just before the end of each cutting plane iteration. ceps is the amount by which the most violated constraint found in the current iteration was violated. cached_constraint is true if the added constraint was constructed from the cache. If the return value is FALSE, then the algorithm is allowed to terminate. If it is TRUE, the algorithm will keep iterating even if the desired precision sparm->epsilon is already reached. */
  return(0);
}

void print_struct_learning_stats(SAMPLE sample, STRUCTMODEL *sm, CONSTSET cset, double *alpha, STRUCT_LEARN_PARM *sparm)
{
  /* This function is called after training and allows final touches to
     the model sm. But primarly it allows computing and printing any
     kind of statistic (e.g. training error) you might want. */
}

void print_struct_testing_stats(SAMPLE sample, STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm, STRUCT_TEST_STATS *teststats)
{
  /* This function is called after making all test predictions in
     svm_struct_classify and allows computing and printing any kind of
     evaluation (e.g. precision/recall) you might want. You can use
     the function eval_prediction to accumulate the necessary
     statistics for each prediction. */
}

void eval_prediction(long exnum, EXAMPLE ex, LABEL ypred, STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm, STRUCT_TEST_STATS *teststats)
{
  /* This function allows you to accumlate statistic for how well the
     predicition matches the labeled example. It is called from
     svm_struct_classify. See also the function
     print_struct_testing_stats. */
  if(exnum == 0) { /* this is the first time the function is
		      called. So initialize the teststats */
  }
}

void write_struct_model(char *file, STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm)
{
  /* Writes structural model sm to file file. */
  FILE *modelfl;
  long j,i,sv_num;
  MODEL *model=sm->svm_model;
  SVECTOR *v;

  if ((modelfl = fopen (file, "w")) == NULL)
  { perror (file); exit (1); }
  fprintf(modelfl,"SVM-multiclass Version %s\n",INST_VERSION);
  fprintf(modelfl,"%d # number of classes\n", sparm->num_classes);
  fprintf(modelfl,"%d # number of base features\n", sparm->num_features);
  fprintf(modelfl,"%d # loss function\n", sparm->loss_function);
  fprintf(modelfl,"%ld # kernel type\n", model->kernel_parm.kernel_type);
  fprintf(modelfl,"%ld # kernel parameter -d \n", model->kernel_parm.poly_degree);
  fprintf(modelfl,"%.8g # kernel parameter -g \n", model->kernel_parm.rbf_gamma);
  fprintf(modelfl,"%.8g # kernel parameter -s \n", model->kernel_parm.coef_lin);
  fprintf(modelfl,"%.8g # kernel parameter -r \n", model->kernel_parm.coef_const);
  fprintf(modelfl,"%s# kernel parameter -u \n",model->kernel_parm.custom);
  fprintf(modelfl,"%ld # highest feature index \n",model->totwords);
  fprintf(modelfl,"%ld # number of training documents \n",model->totdoc);
  fprintf(modelfl,"%ld # sizePsi \n", sm->sizePsi);
  int k;
  for (k = 1; k < sm->sizePsi + 1; k++) {
    fprintf(modelfl,"%lf ", sm->w[k]);
  }
  fprintf(modelfl,"# weight vector (index 1 to sizePsi)\n");
 
  sv_num=1;
  for(i=1;i<model->sv_num;i++) {
   for(v=model->supvec[i]->fvec;v;v=v->next) 
      sv_num++;
  }
  fprintf(modelfl,"%ld # number of support vectors plus 1 \n",sv_num);
  fprintf(modelfl,"%.8g # threshold b, each following line is a SV (starting with alpha*y)\n",model->b);

  for(i=1;i<model->sv_num;i++) {
    for(v=model->supvec[i]->fvec;v;v=v->next) {
      fprintf(modelfl,"%.32g ",model->alpha[i]*v->factor);
      fprintf(modelfl,"qid:%ld ",v->kernel_id);
      for (j=0; (v->words[j]).wnum; j++) {
  fprintf(modelfl,"%ld:%.8g ",
    (long)(v->words[j]).wnum,
    (double)(v->words[j]).weight);
      }
      if(v->userdefined)
  fprintf(modelfl,"#%s\n",v->userdefined);
      else
  fprintf(modelfl,"#\n");
    /* NOTE: this could be made more efficient by summing the
       alpha's of identical vectors before writing them to the
       file. */
    }
  }
}

STRUCTMODEL read_struct_model(char *file, STRUCT_LEARN_PARM *sparm)
{
  /* Reads structural model sm from file file. This function is used
     only in the prediction module, not in the learning module. */
  FILE *modelfl;
  STRUCTMODEL sm;
  long i,queryid,slackid;
  double costfactor;
  long max_sv,max_words,ll,wpos;
  char *line,*comment;
  WORD *words;
  char version_buffer[100];
  MODEL *model;

  nol_ll(file,&max_sv,&max_words,&ll); /* scan size of model file */
  max_words+=2;
  ll+=2;

  words = (WORD *)my_malloc(sizeof(WORD)*(max_words+10));
  line = (char *)my_malloc(sizeof(char)*ll);
  model = (MODEL *)my_malloc(sizeof(MODEL));

  if ((modelfl = fopen (file, "r")) == NULL)
  { perror (file); exit (1); }

  fscanf(modelfl,"SVM-multiclass Version %s\n",version_buffer);
  if(strcmp(version_buffer,INST_VERSION)) {
    perror ("Version of model-file does not match version of svm_struct_classify!"); 
    exit (1); 
  }
  fscanf(modelfl,"%d%*[^\n]\n", &sparm->num_classes);  
  fscanf(modelfl,"%d%*[^\n]\n", &sparm->num_features);  
  fscanf(modelfl,"%d%*[^\n]\n", &sparm->loss_function);  
  fscanf(modelfl,"%ld%*[^\n]\n", &model->kernel_parm.kernel_type);  
  fscanf(modelfl,"%ld%*[^\n]\n", &model->kernel_parm.poly_degree);
  fscanf(modelfl,"%lf%*[^\n]\n", &model->kernel_parm.rbf_gamma);
  fscanf(modelfl,"%lf%*[^\n]\n", &model->kernel_parm.coef_lin);
  fscanf(modelfl,"%lf%*[^\n]\n", &model->kernel_parm.coef_const);
  fscanf(modelfl,"%[^#]%*[^\n]\n", model->kernel_parm.custom);

  fscanf(modelfl,"%ld%*[^\n]\n", &model->totwords);
  fscanf(modelfl,"%ld%*[^\n]\n", &model->totdoc);
  fscanf(modelfl,"%ld%*[^\n]\n", &(sm.sizePsi));
  sm.w = (double *)my_malloc((sm.sizePsi + 1) * sizeof(double));
  double temp = 0.2;
  sm.w[0] = 0.0;
  int k;
  printf("temp:\n");
  for (k = 0; k < sm.sizePsi; k++) {
    fscanf(modelfl,"%lf", &(sm.w[1 + k]));
  }
  fscanf(modelfl,"%*[^\n]\n");
  fscanf(modelfl,"%ld%*[^\n]\n", &model->sv_num);
  fscanf(modelfl,"%lf%*[^\n]\n", &model->b);

  model->supvec = (DOC **)my_malloc(sizeof(DOC *)*model->sv_num);
  model->alpha = (double *)my_malloc(sizeof(double)*model->sv_num);
  model->index=NULL;
  model->lin_weights=NULL;

  for(i=1;i<model->sv_num;i++) {
    fgets(line,(int)ll,modelfl);
    if(!parse_document(line,words,&(model->alpha[i]),&queryid,&slackid,
           &costfactor,&wpos,max_words,&comment)) {
      printf("\nParsing error while reading model file in SV %ld!\n%s",
       i,line);
      exit(1);
    }
    model->supvec[i] = create_example(-1,0,0,0.0,
              create_svector(words,comment,1.0));
    model->supvec[i]->fvec->kernel_id=queryid;
  }
  fclose(modelfl);
  free(line);
  free(words);
  if(verbosity>=1) {
    fprintf(stdout, " (%d support vectors read) ",(int)(model->sv_num-1));
  }
  sm.svm_model=model;
  return(sm);
}

void write_label(FILE *fp, LABEL y)
{
  /* Writes label y to file handle fp. */
  if (fp == NULL) {
    perror ("Error: File cannot be opened in write_label!");
    exit (1);
  }
  int i;
  for (i = 0; i < y.length; i++) {
    fprintf(fp, "%d ", y.labels[i]);
  }
  fprintf(fp, "\n");
} 

void free_pattern(PATTERN x) {
  /* Frees the memory of x. */
  int i;
  for (i = 0; i < x.length; i++) {
    free(x.features[i]);
  }
  free(x.features);
}

void free_label(LABEL y) {
  /* Frees the memory of y. */
  free(y.labels);
}

void free_struct_model(STRUCTMODEL sm) 
{
  /* Frees the memory of model. */
  /* if(sm.w) free(sm.w); */ /* this is free'd in free_model */
  if(sm.svm_model) free_model(sm.svm_model,1);
  /* add free calls for user defined data here */
}

void free_struct_sample(SAMPLE s)
{
  /* Frees the memory of sample s. */
  int i;
  for(i=0;i<s.n;i++) { 
    free_pattern(s.examples[i].x);
    free_label(s.examples[i].y);
  }
  free(s.examples);
}

void print_struct_help()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_learn. */
  printf("         --* string  -> custom parameters that can be adapted for struct\n");
  printf("                        learning. The * can be replaced by any character\n");
  printf("                        and there can be multiple options starting with --.\n");
}

void parse_struct_parameters(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      case 'a': i++; /* strcpy(learn_parm->alphafile,argv[i]); */ break;
      case 'e': i++; /* sparm->epsilon=atof(sparm->custom_argv[i]); */ break;
      case 'k': i++; /* sparm->newconstretrain=atol(sparm->custom_argv[i]); */ break;
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
	       exit(0);
      }
  }
}

void print_struct_help_classify()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_classify. */
  printf("         --* string -> custom parameters that can be adapted for struct\n");
  printf("                       learning. The * can be replaced by any character\n");
  printf("                       and there can be multiple options starting with --.\n");
}

void parse_struct_parameters_classify(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- for the
     classification module */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      /* case 'x': i++; strcpy(xvalue,sparm->custom_argv[i]); break; */
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
	       exit(0);
      }
  }
}
