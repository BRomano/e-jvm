import java.lang.Thread;

public class ThreadTest extends Thread
{
    public ThreadTest()
    {
    }

    public static void main (String[] args)
    {
        ThreadTest t = new ThreadTest();
        t.start();
    }

    public void run()
    {
        String s = new String();
        s = "lulu";
    }
}
