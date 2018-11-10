     #include <stdio.h> 
    int Load() { 
	     FILE *in = fopen("C:\\input\\input.txt", "r"); 
	     int result; 
	     while (fscanf(in, "%d", &result) != EOF) { } 
	     fclose(in);
	     return result; 
     } 
     
     void Store(int data) { 
	     FILE *out = fopen("C:\\input\\input.txt", "a"); 
	     fprintf(out, "\n%d", data); 
	     fclose(out); 
     } 
     int Add(int i, int j) { 
	     return i + j; 
     } 
     void sum() { 
	     for (int i = 0; i < 1000; i++) { 
		     int x = Load(); 
		     x = Add(x, 1); 
		     Store(x); 
		 } 
     }
      
     int main() { 
	     sum(); 
	     return 0; 
     }
