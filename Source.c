#include <stdio.h>
#include <stdlib.h>
#define K 30
struct INFO
{
	int BarcodeNUM[12],BarcodeNUMplace;
	int width, height, BitsPerPixel, FirstColorPix, MiddleColorPix, EndColorPix;
	int MemorySize;
}BMPinfo;
typedef struct nextstore { // This structure helps representing a path for a driver
	int distTotal;	//accumulating distance in a path
	int packCount;	//packages for a store
	int packTotal;	//accumulating packages in a path
	int store;		//a number 0-10
	struct nextstore *next;
}path_t;
typedef struct stores { // This structure helps represent a map
	int num; // store/factory number 0-10
	int exists; // 1 - this store exists in map.txt, 0 - does not exists.
	int r1, r2, r3;
	struct stores *c1;
	struct stores *c2;
	struct stores *c3;
} store;

void fuelConsumption(path_t *path) {//Linoy Levy.
	float fc[K][3];	// holds numeric fuel consumption calculations. K is predefined for the needed accuracy
	float v = 60; // 60 Km/hr
	float dt = 0, t_elapsed = 0/*Starting from time zero*/, a = 0.1, beta = 0.5;
	path_t *temp = path;
	int in_fuel = getFuel();
	int i = 0, dist = 0;
	while (temp != NULL)//while the number of packages is different than zero
	{
		dist = temp->distTotal - dist;//The latest distance is the total distance minus the distance that has already passed
		printf_s("driving to %d", temp->store);
		/*In order to find the dt I divided the distance in rapidly(speed to double the time evenly through).
		Then I divided it in k to get short periods of time. k is determined in advance to create an appropriate
		level of accuracy*/
		dt = ((1.0*dist) / v) / (1.0*K);
		fc[0][0] = t_elapsed; //The first array member is the time and we srart in time zero.
		fc[0][1] = in_fuel;//The second array member is the fuel and we srart in the fuel that we get it from the function "getFuel".
		fc[0][2] = temp->packTotal;//The thirt array member is the num of packages and we get it from the struct path".
		for (i = 1; i < K; i++)
		{//In order to find the appropriate formula, use the Oiler - Lagrange theorem and add the required regular scalars.
			fc[i][0] = i * dt;
			fc[i][1] = -a * fc[i - 1][1] * (1.0*temp->packTotal)*dt + fc[i - 1][1] - beta * v*dt;
			fc[i][2] = temp->packTotal;
			printf_s("%.3f  %.1f \t %d", fc[i][0], fc[i][1], fc[i][2]);
		}
		t_elapsed = t_elapsed + i * dt;//Promotes time
		in_fuel -= fc[K - 1][1]; //The fuel consumed is removed from the general fuel
		temp = temp->next;
	}

}
int getFuel() {//Linoy Levy.
	//get input from user to set truck fuel and cleare stdin buffer @return - users input in the required range

	
	int in_fuel = 0, flag = 1;
	int i;
	char in[128];
	//i did flag =1 and it is mean that the first while have to be
	while (flag) {
		printf_s("%s\n", "Set fuel in the truck's tank. Enter an integer number from 0-100");
		fgets(in, 128, stdin);
		/*in this loop i check if the number that i get from the user is legal .
		In this condition, I check whether the input the user has entered is a number and not a character,
		and requires that the number be at least one digit and at most up to 3 digits
		On the first condition, I check whether the length of the number is between 1 and 3 characters */
		if (strlen(in) - 1 > 3 || strlen(in) - 1 < 0)
		{
			printf_s("%s\n", "Illegal input for fuel");
			flag = 1;
		}
		/*here i check that the user will enter numbers and not characters
		if the user write characters and not numbers the flag will be 1, else the flag will be 0*/
		else {
			for (i = 0; i < strlen(in) - 1; i++)
			{
				if (in[i] < '0' || in[i] > '9') {
					flag = 1;
					printf_s("%s\n", "Illegal input for fuel");
					break;
				}
				flag = 0;
			}
			/*if flag ==0  , and it's mean that the user write number with 1-3 digits. than i get a number from the user
			and i check if number is bigger the 100 or smaller than 0. the flag=1 and it will come back to the while */
			if (!flag) {
				sscanf_s(in, "%d", &in_fuel);
				if (in_fuel > 100 || in_fuel < 0)
				{
					flag = 1;
					printf_s("%s\n", "Illegal input for fuel");
				}
				else flag = 0;
			}
		}
	} // Got input successfully
	return in_fuel;
}
void CheckDig()//Itay Harel.
{//Checks if the barcode is valid.
	int CheckDigit, EvenSum =0, OddSum=0,i;//Setting up int.
	for (i = 0; i < 12; i += 2)//Calculating the sum of the odd index barcode numbers.
		OddSum += BMPinfo.BarcodeNUM[i];
	for (i = 1; i < 12; i += 2)
		EvenSum += BMPinfo.BarcodeNUM[i];//Calculating the sum of the eveb index barcode numbers.
	CheckDigit = BMPinfo.BarcodeNUM[11] + EvenSum + OddSum * 3;//Calculating a check digit by formula.
	if (CheckDigit % 10 == 0)//If the CheckDigit is a multiple of 10, the barcode is valid.
		printf_s("FUCK YEA!");
	else//If its not, its not valid.
		printf_s("FUCK NO!");
}
void Padding()//Itay Harel.
{
	
}
void BarCodeHEADER()//Itay Harel.
{
	FILE * BARCODE; //Defining the barcode image.
	fopen_s(&BARCODE, "package.bmp", "rb");//Open the image.
	fseek(BARCODE, 18, SEEK_SET);//Move to the point where the width is stored.
	fread(&BMPinfo.width, 1, 4, BARCODE);//Save it.
	fseek(BARCODE, 22, SEEK_SET);//Move to the point where the height is stored.
	fread(&BMPinfo.height, 1, 4, BARCODE);//Save it.
	fseek(BARCODE, 28, SEEK_SET);//Move to the point where the Bits per pixel is stored.
	fread(&BMPinfo.BitsPerPixel, 1, 4, BARCODE);//Save it.
	BMPinfo.MemorySize = ((BMPinfo.width*BMPinfo.BitsPerPixel + 31) / 32 * BMPinfo.height * 4);//Calculate the array size of the image.
	if (BMPinfo.width == 137 || BMPinfo.width == 141)
		Padding(3);
	if (BMPinfo.width == 138 || BMPinfo.width == 142)
		Padding(2);
	if (BMPinfo.width == 139 || BMPinfo.width == 143)
		Padding(1);
	fclose(BARCODE);
}
char * BarcodeINFOscanner()//Itay Harel.
{
	FILE * BARCODE;//Defining the file.
	int i,j, flag = 0;//Flags and indexes.
	unsigned char **Pixels;//The string that will save the pixels data.
	fopen_s(&BARCODE, "package.bmp", "rb");//Open the file.
	BarCodeHEADER();//This function scanning the header of the bmp file and saves the date.
	Pixels = (char**)malloc(BMPinfo.height*3 * sizeof(char*));//Setting the array that will save the data.
	for (i = 0; i < BMPinfo.height * 3; i++)
		Pixels[i] = (char*)malloc(BMPinfo.width * 3 * sizeof(char));
	fseek(BARCODE, 54, SEEK_SET);//Go the first pixel that has color in it.
	for (i = 0; i < BMPinfo.height; i++) {//The loop the scans the barcode.
		for (j = 0; j < BMPinfo.width; j++) {
			fread(&Pixels[i][j], 1, 1, BARCODE);//Scan one pixel at a time.
			if (Pixels[i][j - 6] == 0 && Pixels[i][j - 5] == 255 && Pixels[i][j - 4] == 255 && Pixels[i][j - 3] == 255 & Pixels[i][j - 2] == 0) {//Scanning for long lines.
				if (flag == 0) {//First long line.
					BMPinfo.FirstColorPix = j - 8;//Save the data.
					flag = 1;//This flag is to make sure it will skip this if.
					continue;//Continue scanning the barcode.
				}
				if (flag == 1) {//Middle long line.
					BMPinfo.MiddleColorPix = j - 8;//Save the data.
					flag = 2;//This flag is to make sure it will skip this if.
					continue;//Continue scanning the barcode.
				}
				if (flag == 2) {//End long line.
					BMPinfo.EndColorPix = j - 8;//Save the data.
					flag = 3;//This flag is to make sure it will skip this if.
					continue;//Continue scanning the barcode.
				}
			}
		}
	}
	fclose(BARCODE);//Closes the file.
	return Pixels;
}
void * FirstBarcodeDig(char Pixels[])//Itay Harel.
{//This function decrypting the first half of the barcode (untill the middle long lines).
	int LineJump = BMPinfo.width * 12, i;//Setting up int. LineJump = 4 lines above the first colored pixel.
	for (BMPinfo.BarcodeNUMplace = 0, i = BMPinfo.FirstColorPix + LineJump + 9; BMPinfo.BarcodeNUMplace < 6;i+=21 ) {//Checks the pixels.
		if (Pixels[i] == -1 && Pixels[i + 3] == -1 && Pixels[i + 6] == -1 && Pixels[i + 9] == 0 && Pixels[i + 12] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 0;//3211
		else if (Pixels[i] == -1 && Pixels[i + 3] == -1 && Pixels[i + 6] == 0 && Pixels[i + 9] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 1;//2221
		else if (Pixels[i] == -1 && Pixels[i + 3] == -1 && Pixels[i + 6] == 0 && Pixels[i + 9] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 2;//2122
		else if (Pixels[i] == -1 && Pixels[i + 3] == 0 && Pixels[i + 6] == 0 && Pixels[i + 9] == 0 && Pixels[i + 12] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 3;//1411
		else if (Pixels[i] == -1 && Pixels[i + 3] == 0 && Pixels[i + 6] == -1 && Pixels[i + 9] == -1 && Pixels[i + 12] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 4;//1132
		else if (Pixels[i] == -1 && Pixels[i + 3] == 0 && Pixels[i + 6] == 0 && Pixels[i + 9] == -1 && Pixels[i + 12] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 5;//1231
		else if (Pixels[i] == -1 && Pixels[i + 3] == 0 && Pixels[i + 6] == -1 && Pixels[i + 9] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 6;//1114
		else if (Pixels[i] == -1 && Pixels[i + 3] == 0 && Pixels[i + 6] == 0 && Pixels[i + 9] == 0 && Pixels[i + 12] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 7;//1312
		else if (Pixels[i] == -1 && Pixels[i + 3] == 0 && Pixels[i + 6] == 0 && Pixels[i + 9] == -1 && Pixels[i + 12] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 8;//1213
		else if (Pixels[i] == -1 && Pixels[i + 3] == -1 && Pixels[i + 6] == -1 && Pixels[i + 9] == 0 && Pixels[i + 12] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 9;//3112
		BMPinfo.BarcodeNUMplace++;//Moving the index.
	}
}
void * SecBarcodeDig(char Pixels[])//Itay Harel.
{//This function decrypting the 2nd half of the barcode. from the middle long lines to the end long lines.
	int LineJump = BMPinfo.width * 12, i;//Setting up int. LineJump = 4 lines above the first colored pixel.
	for (i = BMPinfo.MiddleColorPix + LineJump + 12; i < BMPinfo.EndColorPix + LineJump + 9; i += 21) {
		if (Pixels[i] == 0 && Pixels[i + 3] == 0 && Pixels[i + 6] == 0 && Pixels[i + 9] == -1 && Pixels[i + 12] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 0;//3211
		else if (Pixels[i] == 0 && Pixels[i + 3] == 0 && Pixels[i + 6] == -1 && Pixels[i + 9] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 1;//2221
		else if (Pixels[i] == 0 && Pixels[i + 3] == 0 && Pixels[i + 6] == -1 && Pixels[i + 9] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 2;//2122
		else if (Pixels[i] == 0 && Pixels[i + 3] == -1 && Pixels[i + 6] == -1 && Pixels[i + 9] == -1 && Pixels[i + 12] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 3;//1411
		else if (Pixels[i] == 0 && Pixels[i + 3] == -1 && Pixels[i + 6] == 0 && Pixels[i + 9] == 0 && Pixels[i + 12] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 4;//1132
		else if (Pixels[i] == 0 && Pixels[i + 3] == -1 && Pixels[i + 6] == -1 && Pixels[i + 9] == 0 && Pixels[i + 12] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 5;//1231
		else if (Pixels[i] == 0 && Pixels[i + 3] == -1 && Pixels[i + 6] == 0 && Pixels[i + 9] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 6;//1114
		else if (Pixels[i] == 0 && Pixels[i + 3] == -1 && Pixels[i + 6] == -1 && Pixels[i + 9] == -1 && Pixels[i + 12] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 7;//1312
		else if (Pixels[i] == 0 && Pixels[i + 3] == -1 && Pixels[i + 6] == -1 && Pixels[i + 9] == 0 && Pixels[i + 12] == -1)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 8;//1213
		else if (Pixels[i] == 0 && Pixels[i + 3] == 0 && Pixels[i + 6] == 0 && Pixels[i + 9] == -1 && Pixels[i + 12] == 0)
			BMPinfo.BarcodeNUM[BMPinfo.BarcodeNUMplace] = 9;//3112
		BMPinfo.BarcodeNUMplace++;//Moving the index.
	}
	free(Pixels);//Freeing the array of the pixels.
}
int getOption()
{//This function checks if the menu input is valid.
	char choice;
	scanf_s("%c", &choice);//Input.
	if (choice<'0' || choice >'9') {//If the input isnt a char between 0-9.
		while (getchar() != '\n');//Clears the buffer.
		return 0;//Return 0;
	}
	else
		return choice-48;//Return the number that the user inputs.
}
void option1ReadUPC()//Itay Harel.
{//This function is for the first menu option. 
	FILE * LETSPLAY;
	int i;
	FirstBarcodeDig(BarcodeINFOscanner());
	SecBarcodeDig(BarcodeINFOscanner());
	CheckDig();
	fopen_s(&LETSPLAY, "LetsPlay.txt", "wt");
	for (i = 0; i < 12; i++)
		fprintf_s(LETSPLAY, "%d", BMPinfo.BarcodeNUM[i]);
	fclose(LETSPLAY);
}
void option6FuelConsumption() {//Linoy Levy.
	store *head = NULL;				// create the root of the tree
	path_t *path = NULL;	//set path start from factory
	freeStoreTree(head);
	freePath(path);
}

void main() {
	int choice = 0;
	do {
		printf("--------------------------\nBoNessahek delivery services\n--------------------------\nMenu:\n");
		printf("1. Scan barcode.\n2. Generate barcode.\n3. List Store packages.\n4. Genrate all routes\n5. Generate driver route.\n6. Fuel consumption report.\n7. Students addition\n9. Exit.\nEnter choice: ");
		choice = getOption();
		printf("\n");
		switch (choice) {
		case 1: option1ReadUPC(); break;
		//case 2: option2GenUPC(); break;
		//case 3: option3ListPacks(); break;
		//case 4: option4PrintsPathes(); break;
		//case 5: option5DriverPath(); break;
		case 6: option6FuelConsumption(); break;
		//case 7: printf_s("Student addition here!\n\n"); break;
		//case 9: printf_s("Good bye!\n"); break;
		default: printf("Bad input, try again\n\n");
		}
	} while (choice != 9);
}


