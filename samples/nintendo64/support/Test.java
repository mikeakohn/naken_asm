
public class Test
{
  public static int[] triangle =
  {
    150 << 4, (120 << 4) | 4,
    170 << 4, (170 << 4) | 8,
    110 << 4, (190 << 4) | 12,
  };

  static public void main(String[] args)
  {
    Triangle.compute(triangle);
  }
}

