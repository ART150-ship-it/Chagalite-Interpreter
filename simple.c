// ***************************************************
// * CS460: Programming Assignment 5: Test Program 1 *
// ***************************************************

function int add_three (int a)
{
  printf("Something Else\n");
  return 3 + a;
}

procedure main (void)
{
  printf("Hello, World!\n");
  int b;
  b = 0;
  int c;
  c = 9;
  bool cont;
  cont = TRUE;
  while (cont) {
    printf("Inside while loop %d\n", b);
    b = b + 1;
    if (b == 3) {
      cont = FALSE;
    }
  }
  int i;
  for (i = 0; i < 3; i = i + 1) {
    printf("Inside for loop %d\n", i);
  }

  char str[15];
  str = "Hello";

  printf("My string is %s\n", str);

  int v1;
  int v2;
  int res;

  v1 = 5;

  v2 = 3;

  res = v1 - v2 + 1;

  printf("five minus three plus one is %d\n", res);
  
}
