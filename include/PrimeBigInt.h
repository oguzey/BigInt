#ifndef PRIMEBIGINT_H
#define PRIMEBIGINT_H

#include "BigInt.h"
#include "../logger/logger.h"

class PrimeBigInt : public BigInt {
public:
	PrimeBigInt(): BigInt()
	{
		do {
			generateRand();
		} while (!testSimpleDivision() || !testMillerRabin(3));
	}

private:
	///
	/// \brief testSimpleDivision
	/// \return		true if the number has not dividers, otherwise false.
	///
	bool testSimpleDivision()
	{
		BigInt y, r, q;

		int simplePrimes[] = {2, 3, 5, 7, 11, 13, 17};
		int size = sizeof(simplePrimes) / sizeof(simplePrimes[0]);
		assert(size == 7);
		for (int i = 0; i < size; ++i) {
			y.setNumber(simplePrimes[i]);
			div(y, q, r);
			if (r.isZero()) {
				LOG("Number has divider {}", simplePrimes[i]);
				return false;
			}
		}
		return true;
	}

	bool isDivisor(BigInt &x)
	{
		BigInt r, q;
		div(x, q, r);
		return r.isZero();
	}

	bool testMillerRabin_real(int k)
	{
		BigInt d, one, x, minusOne, res, copyX;

		one.setNumber(1);
		minusOne.copyContent(*this);
		minusOne.sub(one);

		d.copyContent(minusOne);

		block s = 0;
		while (d.getBit(0) == 0) {
			d.shiftRightBit();
			++s;
		}

		for (int i = 0; i < k; ++i) {
			do {
				LOG("Generare new x...");
				x.generateRand();
				x.mod(*this);
			} while (x.cmp(1) != 1);

			if (isDivisor(x)) {
				LOG("Generated x is divisor for p. x = {}, p = {}",
				    x.toString(), toString());
				LOG("P is NOT pseudosimple for base x. (1)");
				return false;
			}
			copyX.copyContent(x);
			x.exp(d, *this, res);
			if (res.isEqual(one) || res.isEqual(minusOne)) {
				LOG("P is pseudosimple for base x. (2))");
				continue;
			}
			for (block r = 1; r < s; ++r) {
				res.mulMont(res, *this, res);
				if (res.isEqual(one)) {
					LOG("P is NOT pseudosimple for base x. (3)");
					return false;
				} else if (res.isEqual(minusOne)) {
					LOG("P is pseudosimple for base x. (3)");
					continue;
				}
			}
			LOG("After all steps P is NOT pseudosimple for base x. (4)");
			return false;
		}
		return true;
	}

	bool testMillerRabin(int k)
	{
		initModularReduction();
		bool res = testMillerRabin_real(k);
		shutDownModularReduction();
		return res;
	}

//	public static bool TestMillerRabin(this BigInteger p, int k)
//          {
//              if (!TestSimpleDivision(p)) return false;

//              var d = p - 1;
//              var s = 0;
//              while ((d & 1) == 0)
//              {
//                  d >>= 1;
//                  s++;
//              }
//              for (int i = 0; i < k; i++)
//              {
//                  var x = GetRandomBigInt(p);
//                  if (BigInteger.GreatestCommonDivisor(p, x) != 1) return false;
//                  var xPowD = BigInteger.ModPow(x, d, p);
//                  if ( !BigInteger.Abs(xPowD).IsOne && xPowD != (p - 1) ) return false;

//                  for (var r = 1; r < s; r++)
//                  {
//                      xPowD = BigInteger.ModPow(xPowD, 2, p);
//                      if (xPowD == p - 1) break;
//                      if (xPowD.IsOne) return false;
//                  }
//              }
//              return true;
//          }
};

#endif // PRIMEBIGINT_H

