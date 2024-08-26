# Mildly faster version 

import sys
import cProfile


class TempReadings:
    def __init__(self, initial_temp):
        self.count = 1
        self.total_temp = initial_temp
        self.min_temp = initial_temp
        self.max_temp = initial_temp

    def add(self, temp_val: int):
        self.count += 1
        self.total_temp += temp_val
        if temp_val < self.min_temp:
            self.min_temp = temp_val
        elif temp_val > self.max_temp:
            self.max_temp = temp_val

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


def line_splitter(s: str) -> tuple[str, str]:
    idx = s.index(';')
    return s[:idx], s[idx + 1:]


def output_location_temps(filename: str) -> dict[str, int]:
    result = {}
    with open(filename, encoding='utf-8', mode='r') as f:
        for this_line in f:
            # format of the row is..
            # La Ceiba;33.7
            # fields = line_splitter(this_line)
            fields = this_line.split(';', maxsplit=1)
            location = fields[0]
            temp = int(float(fields[1]) * 10.0)

            if location not in result:
                result[location] = [1, temp, temp, temp]
            else:
                x = result[location]
                x[0] += 1
                x[1] += temp
                if temp < x[2]:
                    x[2] = temp
                elif temp > x[3]:
                    x[3] = temp

            #print(f"updated with {temp} -> {result[location]}")

    return result


def format_output(temp_dict) -> str:
    s = "{"
    joiner = ""
    for this_city in sorted(temp_dict.keys()):
        x = temp_dict[this_city]
        min_val = x[2]
        max_val = x[3]
        avg_val = round(x[1] / x[0]) / 10.0
        s += f"{joiner}{this_city}={min_val}/{avg_val}/{max_val}"
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
    #cProfile.run('main()')

