for i in range(2, 500):
    with open("tuple"+str(i) +".rs", "w") as f:
        f.write("pub fn main() {")
        f.write("let t = (")
        for x in range(0, i):
            f.write(str(x) + ",")
        f.write(");")
        for x in range(0, i):
            f.write("t." + str(x) + ";")
        f.write("}")

for i in range(2, 500):
    with open("tuple"+str(i) +".circle.cpp", "w") as f:
        f.write("int main() {")
        f.write("template<class... Ts> struct tuple { [[no_unique_address]] Ts... ts; template<auto N> auto get() { return ts...[N]; }}; auto t = tuple{")
        for x in range(0, i):
            f.write(str(x) + ",")
        f.write("};")
        for x in range(0, i):
            f.write("t.get<" + str(x) + ">();")
        f.write("}") 

for i in range(2, 500):
    with open("tuple"+str(i) +".cpp", "w") as f:
        f.write("#include <tuple>\n")
        f.write("int main() {")
        f.write("auto t = std::tuple{")
        for x in range(0, i):
            f.write(str(x) + ",")
        f.write("};")
        for x in range(0, i):
            f.write("std::get<" + str(x) + ">(t);")
        f.write("}")

for i in range(2, 500):
    with open("tuple"+str(i) +".d", "w") as f:
        f.write("import std : tuple;\n")
        f.write("int main() {")
        f.write("auto t = tuple(")
        for x in range(0, i):
            f.write(str(x) + ",")
        f.write(");")
        for x in range(0, i):
            f.write("auto _" + str(x) + " = t[" + str(x) + "];")
        f.write("return 0; }")


for i in range(2, 500):
    with open("tuple"+str(i) +".zig", "w") as f:
        f.write("pub fn main() !void {")
        f.write("const t = .{")
        for x in range(0, i):
            f.write(str(x) + ",")
        f.write("};")
        for x in range(0, i):
            f.write("_ = t[" + str(x) + "];")
        f.write("}")

