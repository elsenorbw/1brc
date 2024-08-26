# Naive version of the 1brc to set the standard 

import sys
import math


def count_file(filename: str) -> int:
    rows = 0
    with open(filename, encoding='utf-8', mode='r') as f:
        for _ in f:
            rows += 1

    print(f"File has {rows} rows.")
    return rows


def output_location_counts(filename: str) -> dict[str, int]:
    result = {}
    with open(filename, encoding='utf-8', mode='r') as f:
        for this_line in f:
            # format of the row is..
            # La Ceiba;33.7
            fields = this_line.split(';')
            location = fields[0]
            if location not in result:
                result[location] = 1
            else:
                result[location] += 1
    print(result)
    return result


class TempReadings:
    def __init__(self):
        self.count = 0
        self.total_temp = 0
        self.min_temp = 100
        self.max_temp = -100

    def add(self, temp_val: int):
        self.count += 1
        self.total_temp += int(temp_val * 10.0)
        self.min_temp = min(self.min_temp, temp_val)
        self.max_temp = max(self.max_temp, temp_val)

    def get_values(self) -> tuple[float, float, float]:
        avg = round(self.total_temp / self.count) / 10.0
        min = self.min_temp * 1.0
        max = self.max_temp * 1.0
        return avg, min, max

    def __str__(self) -> str:
        avg, min, max = self.get_values()
        s = f"{min}/{avg}/{max}"
        return s

    def raw(self) -> str:
        avg, min, max = self.get_values()
        s = f"count={self.count},total={self.total_temp},divided={self.total_temp / self.count},min={min}/avg={avg}/max={max}"
        return s



def output_location_temps(filename: str) -> dict[str, int]:
    result = {}
    with open(filename, encoding='utf-8', mode='r') as f:
        for this_line in f:
            # format of the row is..
            # La Ceiba;33.7
            fields = this_line.split(';')
            location = fields[0]
            temp = float(fields[1])

            if location not in result:
                result[location] = TempReadings()
            result[location].add(temp)
            #print(f"updated with {temp} -> {result[location].raw()}")

    return result


def format_output(temp_dict) -> str:
    s = "{"
    joiner = ""
    for this_city in sorted(temp_dict.keys()):
        s += f"{joiner}{this_city}={str(temp_dict[this_city])}"
        joiner = ", "
    s += "}"
    return s


def main():
    input_filename = 'measurements.txt'
    if len(sys.argv) > 1:
        input_filename = sys.argv[1]
    temps = output_location_temps(input_filename)
    answer = format_output(temps)
    print(answer)


if __name__ == "__main__":
    main()

