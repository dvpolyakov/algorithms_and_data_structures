#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>

using std::vector;

vector<int> ReadData(std::istream& input_stream = std::cin) {
    int size;
    input_stream >> size;
    vector<int> data(size);
    for (auto& elem : data) {
        input_stream >> elem;
    }
    return data;
}

template <typename Iterator, typename Comparator = std::less<> >
void InsertionSort(Iterator left, Iterator right, Comparator compare={}) {
    while (left < right) {
        auto min_element_it = std::min_element(left, right, compare);
        std::iter_swap(min_element_it, left);
        ++left;
    }
}

template <typename InputItFirst, typename InputItSecond, typename OutputIt,
        typename Comparator = std::less<> >
OutputIt Merge(InputItFirst left, InputItFirst left_end,
           InputItSecond right, InputItSecond right_end,
           OutputIt result, Comparator compare={}) {

    while (left < left_end) {
        if ((right == right_end) || (compare(*left, *right))) {
            *result = std::move(*left);
            ++left;
        } else {
            *result = std::move(*right);
            ++right;
        }
        ++result;
    }
    return std::move(right, right_end, result);
}

template <typename InputIt, typename OutputIt, typename Comparator = std::less<> >
void MergeSort(InputIt left, InputIt right, OutputIt buffer_left, Comparator compare={}) {
    auto size = static_cast<int>(std::distance(left, right));
    if (size < 10) {
        InsertionSort(left, right, compare);
        return;
    }
    auto middle = std::next(left, size / 2);
    MergeSort(left, middle, buffer_left, compare);
    MergeSort(middle, right, buffer_left, compare);
    Merge(left, middle, middle, right, buffer_left, compare);
    std::move(buffer_left, std::next(buffer_left, size), left);
}

template <typename InputIt, typename Comparator = std::less<> >
void Sort(InputIt left, InputIt right, Comparator compare = {}) {
    vector<typename std::iterator_traits<InputIt>::value_type>
            buffer(std::distance(left, right));
    MergeSort(left, right, buffer.begin(), compare);
}

void PrintAnswer(const vector<int>& sorted_data, std::ostream& output_stream = std::cout) {
    for (const auto& elem : sorted_data) {
        output_stream << elem << " ";
    }
    output_stream << std::endl;
}

int main() {
    const auto& data = ReadData();
    Sort(data.begin(), data.end());
    PrintAnswer(data);
    return 0;
}