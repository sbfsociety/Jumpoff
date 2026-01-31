#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <ctime>
bool collided = false;

struct Vec2{
    float x, y;
};



class Player{
    private:
        int health = 100;
        Vec2 speed = {3.f, 0.f};
        Vec2 pos;
        Vec2 gravity = {1.f, 1.f};
    public:
        Player(Vec2 _p) : pos(_p){};
        bool right = true;
        bool air = true;
        bool space = false;
        void Jump(){
            speed.y -= 15.f;
        }
        
        void Move(){
            // if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){newpos.y-=speed.y;}
            // if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)){newpos.y+=speed.y;}
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){pos.x-=speed.x; right = false;}
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){pos.x+=speed.x; right = true;}
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !(air) && collided){Jump(); air = true;}
            pos.y += speed.y;
            if(air){speed.y += gravity.y; space = true;}
                
            space = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)); 
        }

        

        void Draw(sf::RenderWindow& window){
            sf::CircleShape Player(10);
            Player.setFillColor(sf::Color::Red);
            Player.setOrigin({10.f, 10.f});
            Player.setPosition({pos.x, pos.y});
            window.draw(Player);
        }

        Vec2 GetSpeed() const {return speed;}

        void SetSpeed(Vec2 _s) {speed = _s;}
        
        void Set(Vec2 _p) {pos = _p;}

        Vec2 Get() const {return pos;}
        
};

bool checkCollision(Player& player, Vec2 begin, Vec2 end) {
    Vec2 circle = player.Get();
    float radius = 10.0f;

    // 1. Vektor linije (od početka do kraja)
    float dx = end.x - begin.x;
    float dy = end.y - begin.y;
    float lineLenSq = dx*dx + dy*dy;

    // 2. Projekcija centra kruga na beskonačnu liniju (parametar t)
    // t = [(C - begin) dot (end - begin)] / |end - begin|^2
    float t = ((circle.x - begin.x) * dx + (circle.y - begin.y) * dy) / lineLenSq;

    // 3. Ograničavanje t na [0, 1] da bi tačka ostala na segmentu duži
    t = std::max(0.0f, std::min(1.0f, t));

    // 4. Pronalaženje najbliže tačke na duži
    float closestX = begin.x + t * dx;
    float closestY = begin.y + t * dy;

    // 5. Provera distance do najbliže tačke (koristimo kvadrate radi brzine)
    float distSq = (circle.x - closestX) * (circle.x - closestX) + 
                   (circle.y - closestY) * (circle.y - closestY);

    return distSq <= (radius * radius);
}

class Line{
    public:
        bool horizontal = false;
        bool vertical = false;
        bool diagonal = false;

        Vec2 begin;
        Vec2 end;

        Line(Vec2 _b, Vec2 _e){
            this->begin.x = _b.x;
            this->begin.y = _b.y;
            this->end.x = _e.x;
            this->end.y = _e.y;
            if(_b.y == _e.y){this->horizontal = true; std::cout << "This line is horizontal.";}
            else if(_b.x == _e.x){this->vertical = true;std::cout << "This line is vertical.";}
            else{this->diagonal = true;std::cout << "This line is diagonal.";}
        }
        void Collide(Player& player){
                Vec2 pos = player.Get();
                
                if(horizontal){
                    if((abs(pos.y - begin.y) < 10) && pos.x > begin.x && pos.x < end.x){player.Set({pos.x, begin.y - 10}); player.air = false; player.SetSpeed({player.GetSpeed().x, 0.f}); collided = true; /*std::cout << "Landed\n";*/}
                }
                if(vertical){
                    if(abs(pos.x - begin.x) < 15 && pos.y < begin.y && pos.y > end.y){
                        //std::cout << "Collided with vertical line\n";
                        if(player.right == true){
                            player.Set({begin.x - 15, pos.y});
                            // collided = true;
                        }
                        else{
                            player.Set({begin.x + 15, pos.y});
                            // collided = true;
                        }
                    }
                }
                if(diagonal){
                    bool collision = checkCollision(player, begin, end);
                    if(collision){
                        //std::cout << "Collided with diagonal line\n";
                        // Approximate the slope of the line
                        float slope = (end.y - begin.y) / (end.x - begin.x);
                        // Determine the y-intercept of the line
                        float intercept = begin.y - slope * begin.x;
                        // Calculate the closest point on the line to the player
                        Vec2 pos = player.Get();
                        float closestX = (slope * (pos.y - intercept) + pos.x) / (slope * slope + 1);
                        float closestY = slope * closestX + intercept;
                        // Adjust player's position based on the slope
                        if(slope > 0){ // Positive slope
                            player.Set({closestX, closestY - 10});
                        }
                        else{ // Negative slope
                            player.Set({closestX, closestY - 10});
                        }
                        player.SetSpeed({player.GetSpeed().x, 0.f});
                        player.air = false;
                        collided = true;
                        }

                
                
                }

            }
        

        void Draw(sf::RenderWindow& window){
            sf::ConvexShape line;
            line.setPointCount(4);
            line.setPoint(0, sf::Vector2f(begin.x, begin.y));
            line.setPoint(1, sf::Vector2f(end.x, end.y));
            if(horizontal){line.setPoint(3, sf::Vector2f(begin.x, begin.y + 3));}
            else{line.setPoint(3, sf::Vector2f(begin.x + 3, begin.y));}
            if(horizontal){line.setPoint(2, sf::Vector2f(end.x, end.y + 3));}
            else{line.setPoint(2, sf::Vector2f(end.x + 3, end.y + 3));}
           
            
            
            line.setFillColor(sf::Color::Red);
            window.draw(line);
        }
};


void Lines(Player& player, sf::RenderWindow& window, std::vector<Line>& lines){
    ::collided = false;
    for(auto& line : lines){
        line.Collide(player);
        line.Draw(window);
        if(!collided){player.air = true;}
        else{player.air = false;}
            
    }


}



int main(){
    sf::RenderWindow window(sf::VideoMode({1000, 800}), "Jump king");
    window.setFramerateLimit(60);
    Player player({450, 400});
    
    
    // LINES ------------------------------------------------------
    
    
    //Line line({300, 500}, {500, 500});
    std::vector<Line> lines;
    
    lines.push_back(Line({300, 500}, {500, 500}));
    lines.push_back(Line({300, 500}, {500, 500}));
    lines.push_back(Line({300, 500}, {300, 300}));
    lines.push_back(Line({500, 500}, {600, 300}));
    //lines.push_back(Line({500, 500}, {500, 300}));

    // LINES ------------------------------------------------------

    while(window.isOpen()){
        while(auto event = window.pollEvent()){
            if(event->is<sf::Event::Closed>())
                window.close();
        }
        player.Move();
        window.clear(sf::Color::Black);
        Lines(player, window, lines);
        player.Draw(window);
        window.display();
    }

    
    
    
    return 0;
}
