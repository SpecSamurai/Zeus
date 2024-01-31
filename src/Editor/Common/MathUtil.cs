namespace Zeus.Common;

public static class MathUtil
{
    public static float Epsilon => 0.00001f;

    public static bool IsTheSameAs(this float a, float b)
    {
        return Math.Abs(a - b) < Epsilon;
    }

    public static bool IsTheSameAs(this float? a, float? b)
    {
        if(!a.HasValue || !b.HasValue) return false;
        return Math.Abs(a.Value - b.Value) < Epsilon;
    }
}
