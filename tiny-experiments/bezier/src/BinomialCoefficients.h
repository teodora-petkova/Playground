class BinomialCoefficients
{
public:
    BinomialCoefficients() {}

    BinomialCoefficients(int n)
    {
        this->coefficients = std::vector<long long>(n);

        for (int i = 0; i < n; i++)
        {
            this->coefficients[i] = 1;
        }

        for (int k = 1; k < n; k++)
        {
            this->coefficients[k] = C(n - 1, k);
        }
    }

    ~BinomialCoefficients()
    {
    }

    int getCoefficient(int i) const
    {
        return this->coefficients[i];
    }

private:
    std::vector<long long> coefficients;

    //https://en.wikipedia.org/wiki/Binomial_coefficient#Multiplicative_formula
    //(n k)= n!/k!(n-k)! = (n(n-1)...(n-k+1))/k! = Multplication{i, 1, k}[(n+i-1)/i]
    int C(int n, int k)
    {
        double result = 1;
        for (int i = 1; i <= k; ++i)
            result = result * (n - i + 1) / i;
        return (int)(result + 0.01);
    }
};