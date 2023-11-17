#ifndef _GUI_EASE_H
#define _GUI_EASE_H

inline double
easeOutBounce(double x)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsequence-point"

  const double n1 = 7.5625;
  const double d1 = 2.75;

  if (x < 1.0/d1)
    return n1*x*x;
  else if (x < 2.0/d1)
    return n1*(x -= 1.5/d1)*x + 0.75;
  else if (x < 2.5/d1)
    return n1*(x -= 2.25/d1)*x + 0.9375;
  else
    return n1*(x -= 2.625/d1)*x + 0.984375;
  
#pragma GCC diagnostic pop
}

inline double
easeInBounce(double x)
{
  return 1.0 - easeOutBounce(1.0 - x);
}

#endif /* _GUI_EASE_H */
