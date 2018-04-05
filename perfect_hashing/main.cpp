#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <experimental/optional>

using std::vector;

class LinearHashFunction;

class FixsdSet;

class FirstLevelHashFunction;

class Bucket;

int Mod(int64_t lhs, int64_t rhs);

class LinearHashFunction {
public:
    static const int kDefaultPrime = 2147483647;
    LinearHashFunction() : coeff_(1), intercept_(0), module_(kDefaultPrime) {};
    LinearHashFunction(int coeff, int intercept, int module = kDefaultPrime)
            : coeff_(coeff), intercept_(intercept), module_(module) {};

    template <class RandomGenerator>
    static LinearHashFunction GenarateRandomHash(RandomGenerator& rd_generator,
                                                 int module = kDefaultPrime);
    int64_t operator ()(int key) const;

private:
    int coeff_;
    int intercept_;
    int module_;
};

int64_t LinearHashFunction::operator()(int key) const {
    return Mod(static_cast<int64_t>(key) * coeff_ + intercept_, module_);
}

template <class RandomGenerator>
LinearHashFunction LinearHashFunction::GenarateRandomHash(RandomGenerator& rd_generator,
                                                          int module) {
    int coeff = std::uniform_int_distribution<int>(0, module - 1) (rd_generator);
    int intercept = std::uniform_int_distribution<int>(1, module - 1) (rd_generator);
    return {coeff, intercept, module};
}

class FixedSet {
public:
    FixedSet() {};
    void Initialize(const vector<int> &numbers);
    bool Contains(int element) const;
    const int kMaxSizeRatio = 2;
    LinearHashFunction hash_function_;

private:
    vector<Bucket> buckets_;
    vector<vector<int>> GroupToBuckets(const vector<int>& numbers);
};

class Bucket {
public:
    Bucket() {}
    void Initialize(const vector<int> &, std::mt19937 &);
    bool Contains(int elem) const;

private:
    vector<std::experimental::optional<int>> cells_;
    LinearHashFunction hash_function_;
    void FillBucket(const vector<int> &numbers_in_bucket);
};

int64_t CalculateSquaredSizesSum(const vector<int>& data);

vector<int> CalculateCollisions(const vector<int>& numbers, const LinearHashFunction& hash_function,
                                int size_multiplier);

LinearHashFunction GenerateHashFunction(const std::vector<int>& numbers,
                                        int level_collision_criteria_multiplier,
                                        int buckets_number_multiplier,
                                        std::mt19937& random_generator) {
    LinearHashFunction hashFunction;
    do {
        hashFunction = LinearHashFunction::GenarateRandomHash(random_generator);
    } while (CalculateSquaredSizesSum(CalculateCollisions(
            numbers, hashFunction, buckets_number_multiplier)) >
             static_cast<int>(numbers.size()) * level_collision_criteria_multiplier);
    return hashFunction;
}

void Bucket::Initialize(const vector<int>& numbers_in_bucket,
                        std::mt19937& random_generator) {
    if (numbers_in_bucket.empty()) {
        return;
    }
    hash_function_ = GenerateHashFunction(numbers_in_bucket, 1,
                                          static_cast<int>(numbers_in_bucket.size()),
                                          random_generator);
    FillBucket(numbers_in_bucket);
}

void Bucket::FillBucket(const vector<int>& numbers_in_bucket) {
    cells_.resize(numbers_in_bucket.size() * numbers_in_bucket.size());
    for (auto key : numbers_in_bucket) {
        auto hash_of_key = Mod(hash_function_(key), static_cast<int>(cells_.size()));
        cells_[hash_of_key] = key;
    }
}

bool Bucket::Contains(int elem) const
{
    if (cells_.empty()) {
        return false;
    }
    auto hash_of_key = Mod(hash_function_(elem), static_cast<int>(cells_.size()));
    return cells_[hash_of_key] == elem;
}

void FixedSet::Initialize(const vector<int> &numbers) {
    std::mt19937 generator(time(0));
    hash_function_ = GenerateHashFunction(numbers, kMaxSizeRatio, 1, generator);
    const auto& temporal_buckets_ = GroupToBuckets(numbers);
    buckets_.resize(numbers.size());
    for (size_t index = 0; index < numbers.size(); ++index) {
        buckets_[index].Initialize(temporal_buckets_[index], generator);
    }
}

vector<vector<int>> FixedSet::GroupToBuckets(const vector<int>& numbers) {
    vector<vector<int>> temporal_buckets_(numbers.size());
    for (auto key : numbers) {
        auto hash_of_key = Mod(hash_function_(key),
                               static_cast<int>(temporal_buckets_.size()));
        temporal_buckets_[hash_of_key].push_back(key);
    }
    return temporal_buckets_;
}

bool FixedSet::Contains(int element) const {
    auto bucket_number = Mod(hash_function_(element),
                             static_cast<int>(buckets_.size()));
    return buckets_[bucket_number].Contains(element);
}

vector<int> ReadData(std::istream& input_stream = std::cin) {
    int size_of_data;
    input_stream >> size_of_data;
    vector<int> numbers(size_of_data);
    for (auto& elem : numbers) {
        input_stream >> elem;
    }
    return numbers;
}

std::vector<bool> GetQueryAnswers(const std::vector<int>& numbers,
                                  const std::vector<int>& our_queries) {
    FixedSet our_fixed_set;
    our_fixed_set.Initialize(numbers);
    std::vector<bool> answers;
    answers.reserve(our_queries.size());
    for (auto query_int: our_queries) {
        answers.push_back(our_fixed_set.Contains(query_int));
    }
    return answers;
}

void PrintAnswers (const std::vector<bool>& answers, std::ostream& output_stream = std::cout) {
    for (const auto &elem: answers) {
        output_stream << (elem? "Yes" : "No") << '\n';
    }
}

int main()
{
    const auto& data = ReadData();
    const auto& queries = ReadData();
    const auto& answers = GetQueryAnswers(data, queries);
    PrintAnswers(answers);
    return 0;
}

int Mod(int64_t lhs, int64_t rhs) {
    if (lhs < 0) {
        return rhs - ((-lhs) % rhs);
    }
    return static_cast<int>(lhs % rhs);
}

int64_t CalculateSquaredSizesSum(const vector<int>& data) {
    return std::inner_product(data.begin(), data.end(), data.begin(), 0);
}

vector<int> CalculateCollisions(const vector<int>& numbers,
                                const LinearHashFunction& hash_function, int size_multiplier) {
    vector<int> counter(numbers.size() * size_multiplier, 0);
    for (auto elem: numbers) {
        int hash_value = Mod(hash_function(elem), numbers.size());
        ++counter[hash_value];
    }
    return counter;
}
