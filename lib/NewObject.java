
public class NewObject extends Exception
{
	private int i1;
    private int i2;
    
	public NewObject()
	{
    }
    
    public int add()
    {
        return i1 + i2;
    }
    
    public int add(int i, int j)
    {
        return i + j;
    }
}
