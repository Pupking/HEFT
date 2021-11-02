#include<stdio.h>
#include<stdlib.h>

typedef struct task{
	
	int* comp_cost;
	double rank;
	int id;
	int* EST;
	int* EFT;
	int* comm_cost;
	int* parent;
	
}task;

int cmp_rank(const void *a, const void *b) {
		task* t1 = (task*)a;
		task* t2 = (task*)b;
		return (t2->rank) - (t1->rank);
}

int cmp_id(const void *a, const void *b) {
		task* t1 = (task*)a;
		task* t2 = (task*)b;
		return (t1->id) - (t2->id);
}

int find_min_index(int arr[],int n) {
	int temp = arr[0];
	int index = 0;
	for(int i = 0; i < n; i++) {
		if(temp > arr[i]) {
			temp = arr[i];
			index = i;
		}
	}
	return index;
}

int find_min(int arr[],int n) {
	int temp = arr[0];
	int index = 0;
	for(int i = 0; i < n; i++) {
		if(temp > arr[i]) {
			temp = arr[i];
			index = i;
		}
	}
	return temp;
}

int find_id(task arr[], int n,int id) {
	int index = 0;
	for(int i = 0; i < n; i++) {
		if(arr[i].id == id) {
			index = i;
		} 
	}
	return index;
}

int main() {
	
	
	FILE *input;
	int num_tasks;
	int num_procs;
    input = fopen("input.txt","r");
    
    // Taking input
    fscanf(input,"%d",&num_tasks);
    fscanf(input,"%d",&num_procs);
    
    int proc[num_procs];
    int pred[num_tasks][num_tasks];
    int pred_proc[num_tasks];
    
    task* tasks;
    
    // Allocate memory to all variables
    tasks = (task*)malloc(num_tasks*sizeof(task));
    for(int i = 0; i < num_tasks; i++) {
		tasks[i].id = i+1;	
    	tasks[i].comp_cost = (int*)malloc(num_procs*sizeof(int));
    	tasks[i].EST = (int*)malloc(num_procs*sizeof(int));
    	tasks[i].EFT = (int*)malloc(num_procs*sizeof(int));
    	tasks[i].comm_cost = (int*)malloc(num_tasks*sizeof(int));
    	tasks[i].parent = (int*)malloc(num_tasks*sizeof(int));
    }
    
    // Taking computation cost input
    for(int i = 0; i < num_procs; i++) {
    	for(int j = 0; j < num_tasks; j++) {
    		fscanf(input,"%d ",&tasks[j].comp_cost[i]);
    	}

    }
    
    // Taking communication cost input
    for(int i = 0; i < num_tasks; i++) {
    	
    	for(int j = 0; j < num_tasks; j++) {
    		fscanf(input,"%d",&tasks[i].comm_cost[j]);
    		pred[i][j] = tasks[i].comm_cost[j];
    	}
    }
    
    // Setting predecessors for all the tasks
    for(int i = 0; i < num_tasks; i++) {
    	for(int j = 0; j < num_tasks; j++) {
    		if(tasks[i].comm_cost[j] != 0) {
    			tasks[j].parent[i] = tasks[i].id;
    		}
    		else {
    			tasks[j].parent[i] = -1;
    		}
    		
    	}
    }
    
    for(int i = 0; i < num_procs; i++) {
    	proc[i] = 0;
    }
    //Rank calculation ///////////////////////////////////////////////////////////
    
    // add the avg waiting time
    for(int i = 0; i < num_tasks; i++) {
    	tasks[i].rank = 0;
    	
    	for(int j = 0; j < num_procs; j++) {
    		tasks[i].rank += tasks[i].comp_cost[j];
    	}
    	
    	tasks[i].rank /= num_procs;
    }
    double temp, temp2 = 0;
    
    // add max of sum of previous ranks and communication cost
    for(int i = num_tasks-2 ; i >= 0; i--) {

    	for(int j = num_tasks-1 ; j >= 0; j--) {
    	
    		if(tasks[i].comm_cost[j] != 0) {
    		temp = tasks[i].rank + tasks[j].rank + tasks[i].comm_cost[j];
    			if(temp2 < temp) {
    				temp2 = temp;
				}
			}
    	}
    	tasks[i].rank = temp2;
    	temp2 = 0;
    }
    
    /////////////////// Completed rank calculation
    
    
    // Output before EST and EFT are calculated
    printf("No. of tasks:%d\n",num_tasks);
    printf("No. of processors:%d\n\n",num_procs);
    
    printf("The upward rank values:\n");
    for(int i = 0; i < num_tasks; i++) {
    	printf("Task %d: %lf\n",tasks[i].id,tasks[i].rank);
    }
    printf("\n");
    
	qsort(tasks, num_tasks, sizeof(task), cmp_rank);
    
    printf("\nThe order of the tasks to be scheduled:\n");
    for(int i = 0; i < num_tasks; i++) {
    	printf("%d ",tasks[i].id);
    }
    printf("\n\n");
    
   
    // Calculating EST and EFT
    
    // Finding EST and EFT for entry task
    for(int i = 0; i < num_procs; i++) {
    	tasks[0].EST[i] = 0;
    	tasks[0].EFT[i] = tasks[0].EST[i] + tasks[0].comp_cost[i];
    }
    
    
    int min = find_min_index(tasks[0].EFT,num_procs);
    pred_proc[0] = min;
    int time = tasks[0].EFT[min];
    proc[min] = time;
    int max = 0;
    int temp_max = 0;
    int p;
    int id = 0;
    
    // Loop to find EFT and EST
    for(int i = 1; i < num_tasks; i++) {
    	for(int j = 0; j < num_procs; j++) {
    		for(int k = 0; k < num_tasks; k++) {
    			p = tasks[i].parent[k];
    			id = find_id(tasks,num_tasks,p); 
    			if(p > 0) {
    				if(pred_proc[id] == j) {
    					temp_max = proc[j];
    				}
    				else {
    					temp_max = find_min(tasks[id].EFT, num_procs) + pred[p-1][tasks[i].id-1];
    				}
    				if(temp_max > max) {
    					max = temp_max;
    					if(proc[j] > max) {
    						max = proc[j];
    					}
    				}
    			}
    		}
    		tasks[i].EST[j] = max;
    		tasks[i].EFT[j] = tasks[i].EST[j] + tasks[i].comp_cost[j];
    		max = 0;
    	}
    	min = find_min_index(tasks[i].EFT,num_procs);
    	pred_proc[i] = min;
    	proc[min] = tasks[i].EFT[min];
    }
    //// Loop ends
    
    int makespan = proc[min];

    qsort(tasks, num_tasks, sizeof(task), cmp_id);

    // The other half of the output
    printf("EST and EFT on different processors\n");
    for(int i = 0; i < num_tasks;i++) {
    	printf("Task: %d\n",tasks[i].id);
    	for(int j = 0; j < num_procs;j++) {
    		printf("processor %d||est: %d eft: %d ||\n",j+1,tasks[i].EST[j],tasks[i].EFT[j]);
    	}
    	printf("\n");
    }
    printf("\nFinal Schedule:\n");
    
    for(int i = 0; i < num_tasks; i++) {
    	int ind = find_min_index(tasks[i].EFT,num_procs);
    	printf("Task %d is executed on processor %d from time %d to %d\n",tasks[i].id,ind+1,tasks[i].EST[ind],tasks[i].EFT[ind]);
    }
    
    printf("\n\nHence, the makespan length from the schedule: %d\n",makespan);
    
    
    
    fclose(input);

	return 0;
}
