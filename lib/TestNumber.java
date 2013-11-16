
public class TestNumber
{
	public int i1;
    public int i2;
    public int i3;
    public String s1;

	public TestNumber()
	{
    }

   public static void main (String[] args) throws NewObject
   {
      TestNumber t = new TestNumber();
      t = new TestNumber();
      t.s1 = "Hello World";
      t.s1 = "Hello Worldxxx";
      TestNumber t1 = t;
      TestNumber t2 = t;
      t = new TestNumber();
      t = null;
   }

   int main() throws NewObject
   {
      TestNumber t = new TestNumber();
      return t.add(1, 2);
   }
   
   public int add(int i1, int i2) throws NewObject
   {
      return i1 + i2;
   }
   
   public float add(float f1, float f2) throws NewObject
   {
      return f1 + f2;
   }

   public float add(int i1, float f1) throws NewObject
   {
      return i1 + f1;
   }
   
   public void createArray()
   {
      int[] i = new int[10];
   }
}
