
public class Test
{
  public static int[] triangle =
  {
    150 << 4, (120 << 4) | (int)(0xf * 15 / 100),
    170 << 4, (170 << 4) | (int)(0xf * 50 / 100),
    110 << 4, (190 << 4) | (int)(0xf * 75 / 100),
  };

  static public void main(String[] args)
  {
    Triangle.compute(triangle);
  }
}

