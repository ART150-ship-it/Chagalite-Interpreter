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
  char b[2];
  b[0] = 3;
  b[1] = 2;
  if (add_three(b[0]) == 6) {
    printf("Inside loop\n");
  }
  
}
