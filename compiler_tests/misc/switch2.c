int g(int t)
{
    int x;
    x=0;
    switch(t)
    {
        case 0:
            x=1;
            break;
        case 1:
            x=x+1;
            break;
        case 2:
            x=2;
        default:
            x=t+1;
    }
    return x;
}
