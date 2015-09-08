#include <stdio.h>
int main(void);
int main()
{
	FILE *fp;
	int y, xt, color;
	fp = fopen("TESTIMGS.IMG", "w");
	for(xt = 0; xt < 8; xt++) {
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x00, 0xF8);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0xFF, 0xFF);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x1F, 0x00);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0xE0, 0x07);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x0F, 0x00);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0xE0, 0x7B);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0xE5, 0xAF);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x20, 0xFD);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x0F, 0x3E);


			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0xF1, 0xF8);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x00, 0x00);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x0F, 0x78);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0xAA, 0x55);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x55, 0xAA);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0xA5, 0x5A);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x5A, 0xA5);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0xA5, 0xA5);
			for(y=0; y<1024; y++)
				fprintf(fp, "%c%c", 0x5A, 0x5A);
		//for(color = 0; color < 128; color++)
		//	fprintf(fp, "%c", color);
	}
	fclose(fp);
	return 0;
}