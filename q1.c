#include <stdio.h>

int main(){
	FILE *f1, *f2;
	char filename[100], c;
	int end;

	printf("Enter a file to be read: ");
	scanf("%s", filename);
	f1 = fopen(filename, "r");

	printf("Enter a file to be written: ");
	scanf("%s", filename);
	f2 = fopen(filename, "w+");

	fseek(f1, 0, SEEK_END);
	end = ftell(f1);

	while(end > 0){
		fseek(f1, --end, SEEK_SET);
		c = getc(f1);
		putc(c, f2);
	}

	printf("File has been written backwards\n");
	fclose(f1);
	fclose(f2);

}