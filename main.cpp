#include <chrono>
#include <iostream>
#include <vector>

#include "lang/lang.h"

int main()
{
    constexpr std::string_view source = R"(
package game;
using system.io;

@packed
@aligned(16)
class Vector3
{
    var x: i32 = 0;
    var y: i32 = 0;
    var z: i32 = 0;
};

class Player
{
    private var position: Vector3 = Vector3({ x = 10, y = 20, z = -10 });
    private var velocity: Vector3 = Vector3();

    public Player(x: f32, y: f32, z: f32) -> Player
    {
        self.position.x = x;
        self.position.y = y;
        self.position.z = z;
    }

    public function move(dx: f32, dy: f32) -> void
    {
        self.position.x += dx;
        self.position.y += dy;
    }

    public static dot(vec1: Vector3, vec2: Vector3) -> f32
    {
        return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
    }
};

class Generic<T>
{
    var value: T;
    public Generic() {}
    public inline function GetValue() -> T
    {
        return self.value;
    }
};

function main() -> u8
{
    var x: u32 = 0;
    x += 1;

    var result: f32 = Player.dot(Vector3(1, 2, 3), Vector3(4, 5, 6));

    var intGeneric: Generic<i32> = new Generic<i32>(5);
    var intValue: i32 = intGeneric.GetValue();

    var stringGeneric: Generic<string> = new Generic<string>();
    stringGeneric.value = "Hello, World!";
    var strValue: string = stringGeneric.GetValue();

    for (var i: u8 = 0; i < 10; i += 1)
    {
        io.write("Hello, World!");
    }

    return 0;
})";

    lexer::lexer_t lexer;
    lexer_init(lexer, source);

    const auto start = std::chrono::high_resolution_clock::now();
    const std::vector<lexer::token_t> tokens = tokenize(lexer);
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "Lexing completed in " << duration << " microseconds." << "\n";
    std::cout << "Total tokens generated: " << tokens.size() << "\n";
    std::cout << "Total line: " << lexer.line << "\n";
    flush_errors(lexer);

    for (const auto& [type, value]: tokens)
    {
        std::cout << "Type: " << static_cast<int>(type) << " Value: " << value << "\n";
    }

    lexer.tokens.clear();
    lexer.tokens.shrink_to_fit();

    return 0;
}
