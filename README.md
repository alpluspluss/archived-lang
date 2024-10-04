# Lumen-Lang

Lumen-Lang is a multi-paradigm, statically-typed programming language designed with a strong emphasis on performance.
The language is designed to be simple, expressive, and easy to learn, while providing little to none
overhead during runtime.

## Example Code

Here’s a quick example of a simple `Vector3` class, a `Player` class, and the use of generic types in Quanta-Lang:

```lumen
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

    const result: f32 = Player.dot(Vector3(1, 2, 3), Vector3(4, 5, 6));

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
}
```

## Roadmap
Lumen plans to release the first version of the language in the near future.
The language is still in development, and the roadmap is as follows:

- [x] Lexer
- [ ] Parser
- [ ] Intermediate Representation & Code Optimization
- [ ] Code Generation
- [ ] VM & Runtime
