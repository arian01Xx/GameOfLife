#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <cstdlib>
#include <random>
#include <vector>

constexpr int TILE=10;

struct World{
    static constexpr int ROWS=70;
    static constexpr int COLS=100;
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
    int total_population=population;
    int females=0, males=0;
    int row=world.size();
    int col=world[0].size();

    std::vector<int> _x={-1,0,1,-1,1,-1,0,1};
    std::vector<int> _y={-1,-1,-1,0,0,1,1,1};
    //std::vector<std::vector<int>> new_w=world;

    void draw(sf::RenderWindow& window){
        sf::RectangleShape cell(sf::Vector2f(TILE-1, TILE-1));
        //cell.setFillColor(sf::Color::Cyan);

        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                if(world[i][j]==1){
                    cell.setFillColor(sf::Color::Blue);
                    cell.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(cell);
                }else if(world[i][j]==2){
                    cell.setFillColor(sf::Color(255,20,147)); //rojo, verde, azul
                    cell.setPosition({float(j*TILE), float(i*TILE)});
                    window.draw(cell);
                }
            }
        }
    }

    void statistic(sf::RenderWindow& window, sf::Text& _po, sf::Text& _males, sf::Text& _females){
        _po.setString("POPULATION: "+std::to_string(total_population)); //  \n es para salto de linea:wq
                                                                        //
        _males.setString("MALES: "+std::to_string(males));
        _females.setString("FEMALES:"+std::to_string(females));

        _po.setPosition(sf::Vector2f(100*TILE, 1*TILE));
        window.draw(_po);

        _males.setPosition(sf::Vector2f(100*TILE, 5*TILE));
        window.draw(_males);

        _females.setPosition(sf::Vector2f(100*TILE, 10*TILE));
        window.draw(_females);
    }

    void run(){
        int _p=population;
        bool male=true;

        std::random_device rd;
        std::mt19937 gen(rd());
        
        std::uniform_int_distribution<> dis1(0, row-1);
        std::uniform_int_distribution<> dis2(0, col-1);

        while(_p!=0){
            x=dis1(gen);
            y=dis2(gen);

            if(male) world[x][y]=1;
            else world[x][y]=2;
            
            male=!male;
            _p--;
        }
    }

    void recalculate(){
        total_population=0;
        males=0;
        females=0;

        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                if(world[i][j]!=0) total_population++;
                if(world[i][j]==1) males++;
                if(world[i][j]==2) females++;
            }
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
                    if(world[new_i][new_j]!=0) neighbors++;
                }

                if(world[i][j]==0 && neighbors==3){
                    int x=(rand()%2)+1;
                    next[i][j]=x;
                }
                if((world[i][j]==1 || world[i][j]==2) && (neighbors>3 || neighbors<2)) next[i][j]=0; //MUERE 
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

    //---------------ZONA TEXT---------------
    sf::Font font;
    if(!font.openFromFile("arial.ttf")) std::cerr<<"No se pudo cargar el archivo\n";

    sf::Text _population(font, "", 30);
    _population.setFillColor(sf::Color::White);

    sf::Text _males(font, "", 30);
    _males.setFillColor(sf::Color::White);

    sf::Text _females(font, "", 30);
    _females.setFillColor(sf::Color::White);
    //---------------------------------------

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

                    if(row>=0 && row<_w.row && col>=0 && col<_w.col){
                        if(_w.world[row][col]==0) _w.world[row][col]=1;
                        else _w.world[row][col]=0;
                    }
                } 
            }
        }
        
        if(!paused){
            if(clock.getElapsedTime().asMilliseconds()>150){ //si aumentas a 500 seria medio segundo
                                                             //el juego se actualiza cada medio segundo
                _w.recalculate();
                _w.elevate();
                clock.restart();
            }
        } 

        window.clear();
        _w.statistic(window, _population, _males, _females);
        _w.draw(window);
        window.display();
    }
}

int main(){

    execute();

    return 0;
}
