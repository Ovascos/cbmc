void main(int argc, char **argv){
    int val = 3;

    val = val + 1;

    switch(val)
    {
      case 3:
        val *= 4;
        break;
      case 4:
        val *= 3;
        break;
      default:
        __CPROVER_assert(0, "unreachable");
    }

    __CPROVER_mut_output(val);
}
