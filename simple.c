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
  b = -12;
  int c;
  c = 9;
  bool cont;
  cont = TRUE;
  while (cont) {
    printf("Inside loop%d\n", b);
    b = b + 1;
    if (b == 10) {
      cont = FALSE;
    }
  }
  
}
