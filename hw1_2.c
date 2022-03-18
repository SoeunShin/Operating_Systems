#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define LENGTH 128
#define BUFFER_SIZE 256

char buffer[BUFFER_SIZE];
int buffer_size = 0;
int buffer_pos = 0;

int ReadTextLine(int fd, char str[], int max_len){
	int i = 0;
	int j = 0;
	int ret = 0;
	
	// if current position is 0, reset buffer size and pos
	if(lseek(fd, 0, SEEK_CUR) == 0)
		buffer_pos = buffer_size = 0;
	
	while(j < max_len-1){
		if(buffer_pos == buffer_size){
			buffer[0] = 0;
			buffer_size = read(fd, buffer, BUFFER_SIZE);
			buffer_pos = 0;
		}
		if(buffer_size == 0){
			if(j == 0)
				ret = EOF;
			break;
		}
		while(j < max_len-2 && buffer_pos < buffer_size){
			str[j++] = buffer[buffer_pos++];
			if(str[j-1] == '\0' || str[j-1] == 10){
				j--;
				max_len = j;
				break;
			}
		}
	}
	str[j] = 0;
	return ret;
}

int main(){
	int fd = 0;
	char in_buffer[BUFFER_SIZE] = {0};

	// cpuinfo
	fd = open("/proc/cpuinfo", O_RDONLY);

	if(fd < 0){
		printf("Failed to open \"%s\" to read.\n", "/proc/cpuinfo");
		return -1;
	}

	char cpu[10], name1[10], name2[10];
	char m1[10], m2[10], m3[10], m4[10], m5[10], m6[10];
	int core;
	for(int i=0; i<18; i++){
		ReadTextLine(fd, in_buffer, BUFFER_SIZE);
		sscanf(in_buffer, "%s %s", name1, name2);
		// cpu cores
		if(strcmp(name1, "cpu")==0 && strcmp(name2, "cores")==0){
			sscanf(in_buffer, "%s %s %s %d", cpu, cpu, cpu, &core);
			break;
		}
		// model name
		if(strcmp(name1, "model")==0 && strcmp(name2, "name")==0){
			sscanf(in_buffer, "%s %s %s %s %s %s %s %s %s", cpu, cpu, cpu, m1, m2, m3, m4, m5, m6);
		}
	}
	printf("# of processor cores = %d\n", core);
	printf("CPU model = %s %s %s %s %s %s\n", m1, m2, m3, m4, m5, m6);
	close(fd);

	// meminfo
	fd = open("/proc/meminfo", O_RDONLY);

	if(fd < 0){
		printf("Failed to open \"%s\" to read.\n", "/proc/meminfo");
		return -1;
	}

	char mem[20];
	int total;
	for(int i=0; i<10; i++){
		ReadTextLine(fd, in_buffer, BUFFER_SIZE);
		sscanf(in_buffer, "%s", mem);

		if(strcmp(mem, "MemTotal:")==0){
			sscanf(in_buffer, "%s %d %s", mem, &total, mem);
			printf("MemTotal = %d\n", total);
			break;
		}
	}
	close(fd);

	// loadavg
	fd = open("/proc/loadavg", O_RDONLY);
	
	if(fd < 0){
		printf("Failed to open \"%s\" to read.\n", "/proc/loadavg");
		return -1;
	}

	float avg1, avg5, avg15;
	ReadTextLine(fd, in_buffer, BUFFER_SIZE);
	sscanf(in_buffer, "%f %f %f", &avg1, &avg5, &avg15);
	printf("loadavg1 = %f, loadavg5 = %f, loadavg15 = %f\n", avg1, avg5, avg15);
	close(fd);

	return 0;
}
