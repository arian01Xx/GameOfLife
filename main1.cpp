#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>

constexpr int TILE=10;

struct World{
    static constexpr int ROWS=70;
    static constexpr int COLS=70;
    std::vector<std::vector<int>> world{
        ROWS, std::vector<int>(COLS, 0)
    };

    /*FORMAS DE INICIALIZAR OBJETOS
     *std::vector<std::vector<int>> world{50, std::vector<int>(50,0)};
      std::vector<std::vector<int>> world = std::vector<std::vector<int>>(50, std::vector<int>(50,0));
      World(): world(50, std::vector<int>(50,0)){}
      World(){ world = std::vector<std::vector<int>>(50, std::vector<int>(50,0)); }

      Pero parentesis fuera de todas las funciones jamàs admite, tanto en class como en struct
      */

    int population=3000;

    int x, y;
    //int prev_x, prev_y;
    int row=world.size();
    int col=world[0].size();

    std::vector<int> _x={-1,0,1,-1,1,-1,0,1};
    std::vector<int> _y={-1,-1,-1,0,0,1,1,1};
    //std::vector<std::vector<int>> new_w=world;

    void draw(sf::RenderWindow& window){
        sf::CircleShape cell(float(TILE-7));
        cell.setFillColor(sf::Color::Cyan);

        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                if(world[i][j]!=0){
                    cell.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(cell);
                }
            }
        }
    }

    void run(){
        int _p=population;

        while(_p!=0){
            x=rand()%(row);
            y=rand()%(col);

            world[x][y]=1;

            _p--;
        }
    } 

    void elevate(){
        auto next=world;

        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                
                int neighbors=0;
                //int new_i, new_j;
                for(int k=0; k<8; k++){
                    int new_i=i+_x[k];
                    int new_j=j+_y[k];
                    if(new_i<0 || new_j<0 || new_i>=row || new_j>=col) continue;
                    neighbors+=world[new_i][new_j];
                }

                if(world[i][j]==0 && neighbors==3) next[i][j]=1; //NACE
                if(world[i][j]==1 && (neighbors>3 || neighbors<2)) next[i][j]=0; //MUERE
                //if(world[i][j]==1 && (neighbors==2 || neighbors==3)) continue;
            }
        }

        world=next;
    }
};

void execute(){
    World _w;
    _w.run();
    bool paused=false;

    sf::RenderWindow window{
        sf::VideoMode({
                static_cast<unsigned>(_w.col*TILE),
                static_cast<unsigned>(_w.row*TILE)
        }), "TETRIS SFML!"
    };

    window.setFramerateLimit(60);

    sf::Clock clock; 

    while(window.isOpen()){
        while(const std::optional event=window.pollEvent()){
            if(event->is<sf::Event::Closed>()) window.close();
            if(const auto* KeyPressed=event->getIf<sf::Event::KeyPressed>()){
                if(KeyPressed->code==sf::Keyboard::Key::Enter) paused= !paused;
            }
            //el de abajo tambien de estar con el if !paused para que el click
            //solo funcione cuando està en pause
            if( const auto* mouseButtonPressed=event->getIf<sf::Event::MouseButtonPressed>()){
                if(mouseButtonPressed->button==sf::Mouse::Button::Left){
                    //sf::Vector2f mousePos=sf::Mouse::getPosition(window);
                    int col=mouseButtonPressed->position.x/TILE;
                    int row=mouseButtonPressed->position.y/TILE;

                    if(row>=0 && row<_w.row && col>=0 && col<_w.col) _w.world[row][col]=1;
                } 
            }
        } 
        
        if(!paused){
            if(clock.getElapsedTime().asMilliseconds()>150){ //si aumentas a 500 seria medio segundo
                                                             //el juego se actualiza cada medio segundo
                _w.elevate();
                clock.restart(); 
            }
        }

        window.clear();
        _w.draw(window);
        window.display();
    }
}

int main(){

    execute();

    return 0;
}

//g++ -o m1 main1.cpp -lsfml-graphics -lsfml-window -lsfml-system
