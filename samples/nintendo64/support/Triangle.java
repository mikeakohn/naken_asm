
public class Triangle
{
  public static void compute(int[] triangle)
  {
    int temp;
    int is_left_major;

    int x0 = triangle[0];
    int y0 = triangle[1];
    int x1 = triangle[2];
    int y1 = triangle[3];
    int x2 = triangle[4];
    int y2 = triangle[5];

    // Sort vertexes so y values go from top to bottom.
    if (y2 < y1)
    {
      temp = y2;
      y2 = y1;
      y1 = temp;

      temp = x2;
      x2 = x1;
      x1 = temp;
    }

    if (y1 < y0)
    {
      temp = y1;
      y1 = y0;
      y0 = temp;

      temp = x1;
      x1 = x0;
      x0 = temp;
    }

    if (y2 < y1)
    {
      temp = y2;
      y2 = y1;
      y1 = temp;

      temp = x2;
      x2 = x1;
      x1 = temp;
    }

    // When y0 == y1 exactly, it can create a division by 0.
    if (y0 == y1) { y1 += 1; }

    // Middle vertex leans to the right (left_major).
    is_left_major = x1 > x0 ? 1 : 0;

    // Slope: y = dy/dx * x + y0
    // Inverse Slope: x = dx/dy * y + x0
    int dx_h = x0 - x2;
    int dx_m = x0 - x1;
    int dx_l = x1 - x2;

    int dy_h = y0 - y2;
    int dy_m = y0 - y1;
    int dy_l = y1 - y2;

    // FIXME: Why shift just dy_h and dy_m by 4?
    // This is here to make sure slope doesn't do a div by 0.
    if (dy_h == 0) { dy_h = 1 << 4; }
    if (dy_m == 0) { dy_m = 1 << 4; }
    if (dy_l == 0) { dy_l = 1; }

System.out.printf("x0=%04x\n", x0);
System.out.printf("x2=%04x\n", x2);
System.out.printf("dx_h=%04x\n", (x0 << 12) - (x2 << 12));
System.out.printf("dx_h=%04x\n\n", dx_h << 12);

System.out.printf("dx_h=%04x\n", dx_h);
System.out.printf("dx_m=%04x\n", dx_m);
System.out.printf("dx_l=%04x\n", dx_l);

    // Shift by 4 is needed for fixed point division.
    int dxhdy = (dx_h << 4) / dy_h;
    int dxmdy = (dx_m << 4) / dy_m;
    int dxldy = (dx_l << 4) / dy_l;

System.out.printf("dy_h=%04x\n", dy_h);
System.out.printf("dy_m=%04x\n", dy_m);
System.out.printf("dy_l=%04x\n", dy_l);

System.out.printf("dxhdy=%04x\n", dxhdy << 12);
System.out.printf("dxmdy=%04x\n", dxmdy << 12);

    // XM is the X coordinate where the middle minor edge hits trunc(y0).
    // XH is the X coordinate where the major edge hits trunc(y0).
    // XL is the X coordinate where the middle minor edge hits YH.YH_2.
    int yh_fraction = y0 & 0xf;
    int xh = x0 - ((dxhdy * yh_fraction) >> 4);
    int xm = x0 - ((dxmdy * yh_fraction) >> 4);
    int xl = x0 + ((dxmdy * (y1 - y0)) >> 4);

    System.out.printf("  .dc64 0x%04x%04x%04x%04x\n",
      (8 << 8) | (is_left_major << 7),
      y2 >> 2,
      y1 >> 2,
      y0 >> 2);

    System.out.printf("  .dc64 0x%04x%04x%04x%04x\n",
      xl >> 4,
      xl << 12 & 0xffff,
      (dxldy >> 4) & 0xffff,
      dxldy << 12 & 0xffff);

    System.out.printf("  .dc64 0x%04x%04x%04x%04x\n",
      xh >> 4,
      xh << 12 & 0xffff,
      (dxhdy >> 4) & 0xffff,
      dxhdy << 12 & 0xffff);

    System.out.printf("  .dc64 0x%04x%04x%04x%04x\n",
      xm >> 4,
      xm << 12 & 0xffff,
      (dxmdy >> 4) & 0xffff,
      dxmdy << 12 & 0xffff);
  }

  private Triangle() { }
}

