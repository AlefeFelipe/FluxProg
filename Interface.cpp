#include "Interface.hpp"

Interface :: Interface()
{

    //variavel que diz se o programa ainda está executando ou não
    executing = true;

    //inicializa o allegro, dá msg de erro caso falhe alguma inicializacao
    if(!al_init()) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao do Allegro", NULL, NULL);
        executing = false;
    }
    if(!al_init_image_addon()) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao do Allegro_image", NULL, NULL);
        executing = false;
    }
    if(!al_init_primitives_addon()) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao do Allegro_primitives", NULL, NULL);
        executing = false;
    }
    if(!al_init_native_dialog_addon()) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao do Allegro_native_dialog", NULL, NULL);
        executing = false;
    }
    if(!al_install_keyboard()) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao do Allegro_keyboard", NULL, NULL);
        executing = false;
    }
    if(!al_install_mouse()) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao do Allegro_mouse", NULL, NULL);
        executing = false;
    }
    if(!al_init_font_addon()) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao do Allegro_font", NULL, NULL);
        executing = false;
    }
    if(!al_init_ttf_addon()) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao do Allegro_ttf", NULL, NULL);
        executing = false;
    }

    //CRIA A JANELA DO PROGRAMA E SETA AS VARIÁVEIS DA JANELA
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_MAXIMIZED);
    display = al_create_display(display_width, display_height);
    al_set_window_position(display, 40, 40);
    al_set_window_title(display, "Fluxprog_v2");
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_set_window_constraints(display, 670, 600, 5000, 5000);
    al_apply_window_constraints(display, true);


    //inicialização das cores
    black = al_map_rgb(0, 0, 0);
    backgroud_color = al_map_rgb(128, 128, 128);
    white = al_map_rgb(255, 255, 255);
    primary_menu_color = al_map_rgb(176, 175, 175);
    blocks_menu_color = al_map_rgb(255, 0, 0);
    functions_menu_color = al_map_rgb(255, 0, 0);
    sensors_menu_color = al_map_rgb(0, 0, 255);

    reset_dragging_variables();

    black_sensor_menu_selected = false;
    color_sensor_menu_selected = false;
    ultrasonic_sensor_menu_selected = false;
    sub_menu = false;
    drawing_line = false;
    temporary_line_X = 0;
    temporary_line_Y = 0;

    // checa se o display foi inicializado corretamente, se não foi dá msg de erro
    if(!display) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao do display", NULL, NULL);
        executing = false;
    }

    // carrega todas as imagens usadas no programa, se alguma não for carregada dá msg de erro
    load_program_images();

    //carrega a fonte, dá msg de erro caso não consiga ser carregada
    font = al_load_font("OpenSans-Regular.ttf", 10, 0);
    if(!font) {
        al_show_native_message_box(NULL, NULL, NULL, "Erro na inicializacao da fonte", NULL, NULL);
        executing = false;
    }
    for(int i=0; i<valor_maximo_blocos; i++) {
        blocks_list_to_print[i] = NULL;
    }

}

Interface :: ~Interface() {

    for(int i=0; i<valor_maximo_blocos; i++) {
        if(blocks_list_to_print[i] != NULL) {
            delete blocks_list_to_print[i];
        }
    }

    //destroi os objetos usados pelo allegro
	destroy_program_images();

	al_destroy_font(font);

	al_shutdown_image_addon();
	al_shutdown_primitives_addon();
	al_shutdown_native_dialog_addon();
	al_shutdown_font_addon();
	al_shutdown_ttf_addon();
    al_destroy_display(display);
}

void Interface :: start() {

    while(executing) {
        //avisa o allegro sobre a mudança de tamanho do display para que as variáveis sejam atualizadas
        al_acknowledge_resize(display);
        //seta a cor de fundo da tela
        al_clear_to_color(backgroud_color);
        //imprime menu
        print_primary_menu();
        //imprime menu de blocos
        print_secondary_menu();

        //checa se o mouse está sobre os menus, para setar a variável de controle do menu_selected
        check_mouse_on_menus();

        //percorre toda a lista de impressão dos blocos
        print_list_of_blocks();

        //desenha objetos sendo arrastados
        draw_dragging();

        draw_temporary_line();

        //atualiza o display
        al_flip_display();

        //cria a lista com eventos
        ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
        al_register_event_source(event_queue, al_get_display_event_source(display));
        al_register_event_source(event_queue, al_get_mouse_event_source());
        al_register_event_source(event_queue, al_get_keyboard_event_source());
        //espera pelo próximo evento
        ALLEGRO_EVENT events;
        al_wait_for_event(event_queue, &events);

        if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            //se a janela for fechada o programa para de executar
            executing = false;
        }

        if(events.type == ALLEGRO_EVENT_MOUSE_AXES) {
            //atualiza as variaveis com as posições do mouse
            mouseX = events.mouse.x;
            mouseY = events.mouse.y;
        }
        if(events.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            //testa se pressionou o mouse sobre algum bloco existente,
            //se sim seta a variável dizendo que o bloco está selecionado para poder arrastá-lo
            for(int i=0; i<valor_maximo_blocos; i++) {
                if(blocks_list_to_print[i] != NULL) {

                    check_mouse_on_points(blocks_list_to_print[i]);

                    if(blocks_list_to_print[i]->getOut1Selected() == true) {
                        temporary_line_X = mouseX;
                        temporary_line_Y = mouseY;
                        drawing_line = true;
                    } else {
                        temporary_line_X = 0;
                        temporary_line_Y = 0;
                        drawing_line = false;
                    }

                    if((mouseX > blocks_list_to_print[i]->getX()) && (mouseX < (blocks_list_to_print[i]->getX()+blocks_list_to_print[i]->getWidth())) && (mouseY > blocks_list_to_print[i]->getY()) && (mouseY < (blocks_list_to_print[i]->getY()+blocks_list_to_print[i]->getHeight()))) {
                        if((blocks_list_to_print[i]->getIn1Selected() == false) && (blocks_list_to_print[i]->getOut1Selected() == false) && (blocks_list_to_print[i]->getIn2Selected() == false) && (blocks_list_to_print[i]->getOut2Selected() == false)) {
                            blocks_list_to_print[i]->setDragging(true);
                            blocks_list_to_print[i]->setSelected(true);
                            mouse_aux_x = mouseX-blocks_list_to_print[i]->getX();
                            mouse_aux_y = mouseY-blocks_list_to_print[i]->getY();
                        }
                    } else {
                        blocks_list_to_print[i]->setSelected(false);
                    }
                }
            }

            switch(menu_selected) {
                case 18:
                    dragging_walk_foward = true;
                    break;
                case 19:
                    dragging_turn_left = true;
                    break;
                case 20:
                    dragging_turn_right = true;
                    break;
                case 24:
                    dragging_black_sensor1 = true;
                    break;
                case 25:
                    dragging_black_sensor2 = true;
                    break;
                case 26:
                    dragging_black_sensor3 = true;
                    break;
                case 27:
                    dragging_black_sensor4 = true;
                    break;
                case 28:
                    dragging_black_sensor5 = true;
                    break;
                case 29:
                    dragging_color_sensor1 = true;
                    break;
                case 30:
                    dragging_color_sensor2 = true;
                    break;
                case 31:
                    dragging_ultrasonic_sensor1 = true;
                    break;
                case 32:
                    dragging_ultrasonic_sensor2 = true;
                    break;
                case 33:
                    dragging_ultrasonic_sensor3 = true;
                    break;
            }

            if(menu_selected != 15) {
                black_sensor_menu_selected = false;
            }
            if(menu_selected != 16) {
                color_sensor_menu_selected = false;
            }
            if(menu_selected != 17) {
                ultrasonic_sensor_menu_selected = false;
            }
        }

        if(events.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            /*
                menu_selected == 1 >> play
                menu_selected == 2 >> pause
                menu_selected == 3 >> stop
                menu_selected == 4 >> save
                menu_selected == 5 >> load
                menu_selected == 6 >> save_as
                menu_selected == 7 >> bluetooth
                menu_selected == 8 >> vrep
                menu_selected == 9 >> bloco de condição
                menu_selected == 10 >> bloco de ação
                menu_selected == 11 >> bloco de inicio
                menu_selected == 12 >> bloco de fim
                menu_selected == 13 >> bloco de junção
                menu_selected == 14 >> bloco de loop
                menu_selected == 15 >> menu de sensor de fita
                menu_selected == 16 >> menu de sensor de cor
                menu_selected == 17 >> menu de sensor de ultrassom
                menu_selected == 18 >> ação de andar
                menu_selected == 19 >> ação de virar esquerda
                menu_selected == 20 >> ação de virar direita
                menu_selected == 21 >> numero 2
                menu_selected == 21 >> lógico V
                menu_selected == 22 >> lógico F
            */
            if(menu_selected == 1) {
                cout<<"play"<<endl;
            }
            if(menu_selected == 2) {
                cout<<"pause"<<endl;
            }
            if(menu_selected == 3) {
                cout<<"stop"<<endl;
            }
            if(menu_selected == 4) {
                cout<<"save"<<endl;
            }
            if(menu_selected == 5) {
                cout<<"load"<<endl;
            }
            if(menu_selected == 6) {
                cout<<"save_as"<<endl;
            }
            if(menu_selected == 7) {
                cout<<"bluetooth"<<endl;
            }
            if(menu_selected == 8) {
                cout<<"vrep"<<endl;
            }
            if(menu_selected == 9) {
                ConditionalBlock *aux = new ConditionalBlock();
                aux->setWidth(al_get_bitmap_width(DECISION_BLOCK[0]));
                aux->setHeight(al_get_bitmap_height(DECISION_BLOCK[0]));
                aux->setTypeOfSensor(0);
                aux->setX(50);
                aux->setY(100);
                add_block(aux);
            }
            if(menu_selected == 10) {
                ActionBlock *aux = new ActionBlock();
                aux->setWidth(al_get_bitmap_width(FUNCTION_BLOCK[0]));
                aux->setHeight(al_get_bitmap_height(FUNCTION_BLOCK[0]));
                aux->setFunction(0);
                aux->setX(50);
                aux->setY(100);
                add_block(aux);
            }
            if(menu_selected == 11) {
                StartBlock *aux = new StartBlock();
                aux->setWidth(al_get_bitmap_width(START_BLOCK[0]));
                aux->setHeight(al_get_bitmap_height(START_BLOCK[0]));
                aux->setX(50);
                aux->setY(100);
                add_block(aux);
            }
            if(menu_selected == 12) {
                EndBlock *aux = new EndBlock();
                aux->setWidth(al_get_bitmap_width(END_BLOCK[0]));
                aux->setHeight(al_get_bitmap_height(END_BLOCK[0]));
                aux->setX(50);
                aux->setY(100);
                add_block(aux);
            }
            if(menu_selected == 13) {
                MergeBlock *aux = new MergeBlock();
                aux->setWidth(al_get_bitmap_width(MERGE_BLOCK[0]));
                aux->setHeight(al_get_bitmap_height(MERGE_BLOCK[0]));
                aux->setX(50);
                aux->setY(100);
                add_block(aux);
            }
            if(menu_selected == 14) {
                LoopBlock *aux = new LoopBlock();
                aux->setWidth(al_get_bitmap_width(LOOP_BLOCK[0]));
                aux->setHeight(al_get_bitmap_height(LOOP_BLOCK[0]));
                aux->setX(50);
                aux->setY(100);
                add_block(aux);
            }
            if(menu_selected == 15) {
                black_sensor_menu_selected = true;
            }
            if(menu_selected == 16) {
                color_sensor_menu_selected = true;
            }
            if(menu_selected == 17) {
                ultrasonic_sensor_menu_selected = true;
            }
            if(menu_selected == 22) {
                cout<<"verdadeiro"<<endl;
            }
            if(menu_selected == 23) {
                cout<<"falso"<<endl;
            }
            //testa se soltou o bloco na lixeira
            for(int i=0; i<valor_maximo_blocos; i++) {
                if(blocks_list_to_print[i] != NULL) {
                    if(blocks_list_to_print[i]->getDragging() == true) {
                        if((mouseX > 0) && (mouseX < 70) && (mouseY > (al_get_display_height(display)-70)) && (mouseY < 0, al_get_display_height(display))) {
                            remove_block(blocks_list_to_print[i]);
                        } else {
                            blocks_list_to_print[i]->setDragging(false);
                        }
                    }
                }
            }
            //checa se soltou alguma funçao ou sensor
            check_dragging();

            reset_dragging_variables();
        }

        if(events.type == ALLEGRO_EVENT_KEY_DOWN) {

            cout<<"tecla apertada: " << events.keyboard.keycode <<endl;
            if((events.keyboard.keycode == 77) || (events.keyboard.keycode == 90)){
                for(int i=0; i<valor_maximo_blocos; i++) {
                    if(blocks_list_to_print[i] != NULL) {
                        if(blocks_list_to_print[i]->getSelected() == true) {
                            remove_block(blocks_list_to_print[i]);
                        }
                    }
                }
            }
        }

    }
}

void Interface :: load_bitmap(ALLEGRO_BITMAP **bitmap, char *adress) {
    //se não achar a imagem no diretorio especificado dá msg de erro e para a execução
    if(!al_load_bitmap(adress)) {
        al_show_native_message_box(NULL, NULL, NULL, adress, NULL, NULL);
        executing = false;
    } else {
        *bitmap = al_load_bitmap(adress);
    }
}

void Interface :: add_block(Block *b) {
    for(int i=0; i<valor_maximo_blocos; i++) {
        if(blocks_list_to_print[i] == NULL) {
            blocks_list_to_print[i] = b;
            cout<< "adicionou na lista" << endl;
            break;
        }
    }

}

void Interface :: remove_block(Block *b) {
    for(int i=0; i<valor_maximo_blocos; i++) {
        if(blocks_list_to_print[i] == b) {
            blocks_list_to_print[i] = NULL;
            delete b;
            cout<< "removeu da lista: " << i << endl;
            break;
        }
    }
}

void Interface :: print_primary_menu() {
    //desenha o retangulo no qual ficam os botões de opções
    al_draw_filled_rectangle(0, 0, al_get_display_width(display), (4 + al_get_bitmap_height(play_button)), primary_menu_color);

    //desenha os botões do menu e checa a posição do mouse para ver se estão selecionados ou não
    if((mouseX < 14 + al_get_bitmap_width(play_button)) && (mouseX > 14) && (mouseY >= 2) && (mouseY <= 2+ al_get_bitmap_height(play_button))) {
        al_draw_bitmap(play_button_selected, 14 + 0*al_get_bitmap_width(play_button), 2, 0);
        menu_selected = 1;
    } else {
        al_draw_bitmap(play_button, 14 + 0*al_get_bitmap_width(play_button), 2, 0);
    }
    if((mouseX < 14 + 2*al_get_bitmap_width(pause_button)) && (mouseX > 14 + al_get_bitmap_width(pause_button)) && (mouseY >= 2) && (mouseY <= 2+ al_get_bitmap_height(pause_button))) {
        al_draw_bitmap(pause_button_selected, 14 + 1*al_get_bitmap_width(pause_button), 2, 0);
        menu_selected = 2;
    } else {
        al_draw_bitmap(pause_button, 14 + 1*al_get_bitmap_width(pause_button), 2, 0);
    }
    if((mouseX < 14 + 3*al_get_bitmap_width(stop_button)) && (mouseX > 14 + 2*al_get_bitmap_width(stop_button)) && (mouseY >= 2) && (mouseY <= 2+ al_get_bitmap_height(stop_button))) {
        al_draw_bitmap(stop_button_selected, 14 + 2*al_get_bitmap_width(stop_button), 2, 0);
        menu_selected = 3;
    } else {
        al_draw_bitmap(stop_button, 14 + 2*al_get_bitmap_width(stop_button), 2, 0);
    }
    if((mouseX < 14 + 4*al_get_bitmap_width(save_button)) && (mouseX > 14 + 3*al_get_bitmap_width(save_button)) && (mouseY >= 2) && (mouseY <= 2+ al_get_bitmap_height(save_button))) {
        al_draw_bitmap(save_button_selected, 14 + 3*al_get_bitmap_width(save_button), 2, 0);
        menu_selected = 4;
    } else {
        al_draw_bitmap(save_button, 14 + 3*al_get_bitmap_width(save_button), 2, 0);
    }
    if((mouseX < 14 + 5*al_get_bitmap_width(load_button)) && (mouseX > 14 + 4*al_get_bitmap_width(load_button)) && (mouseY >= 2) && (mouseY <= 2+ al_get_bitmap_height(load_button))) {
        al_draw_bitmap(load_button_selected, 14 + 4*al_get_bitmap_width(load_button), 2, 0);
        menu_selected = 5;
    } else {
        al_draw_bitmap(load_button, 14 + 4*al_get_bitmap_width(load_button), 2, 0);
    }
    if((mouseX < 14 + 6*al_get_bitmap_width(save_as_button)) && (mouseX > 14 + 5*al_get_bitmap_width(save_as_button)) && (mouseY >= 2) && (mouseY <= 2+ al_get_bitmap_height(save_as_button))) {
        al_draw_bitmap(save_as_button_selected, 14 + 5*al_get_bitmap_width(save_as_button), 2, 0);
        menu_selected = 6;
    } else {
        al_draw_bitmap(save_as_button, 14 + 5*al_get_bitmap_width(save_as_button), 2, 0);
    }

    //desenha o botão de conexão ao v-rep e bluetooth acompanhando o lado direito da tela
    if((mouseX < al_get_display_width(display) + 10 - al_get_bitmap_width(vrep_button) - roll_bar_width) && (mouseX > al_get_display_width(display) - 73.5 - al_get_bitmap_width(bluetooth_button) - roll_bar_width) && (mouseY >= 2) && (mouseY <= 2+ al_get_bitmap_height(bluetooth_button))) {
        al_draw_bitmap(bluetooth_button_selected, al_get_display_width(display) - 73.5 - al_get_bitmap_width(bluetooth_button) - roll_bar_width, 2, 0);
        menu_selected = 7;
    } else {
        al_draw_bitmap(bluetooth_button, al_get_display_width(display) - 73.5 - al_get_bitmap_width(bluetooth_button) - roll_bar_width, 2, 0);
    }
    if((mouseX < al_get_display_width(display) - roll_bar_width) && (mouseX > al_get_display_width(display) + 10 - al_get_bitmap_width(vrep_button) - roll_bar_width) && (mouseY >= 2) && (mouseY <= 2+ al_get_bitmap_height(vrep_button))) {
        al_draw_bitmap(vrep_button_selected, al_get_display_width(display) + 10 - al_get_bitmap_width(vrep_button) - roll_bar_width, 2, 0);
        menu_selected = 8;
    } else {
        al_draw_bitmap(vrep_button, al_get_display_width(display) + 10 - al_get_bitmap_width(vrep_button) - roll_bar_width, 2, 0);
    }

}
void Interface :: print_secondary_menu() {

    int blocks_menu_Y = (al_get_bitmap_height(play_button)+4);
    int sensors_menu_Y = blocks_menu_Y + 15 + 6*al_get_bitmap_height(mini_menu[0]);
    int actions_menu_Y = sensors_menu_Y + 3 * al_get_bitmap_height(WALK_FOWARD_ACTION)+20;
    int extra_menu_Y = actions_menu_Y + 3 * al_get_bitmap_height(WALK_FOWARD_ACTION)+20;

    //desenha o retangulo no qual ficam os botões de opções
    al_draw_filled_rectangle(0, blocks_menu_Y, al_get_bitmap_width(mini_menu[0])+5, blocks_menu_Y + 6*al_get_bitmap_height(mini_menu[0])+5, primary_menu_color);
    al_draw_filled_rectangle(0, sensors_menu_Y, al_get_bitmap_width(mini_menu[0])+5, sensors_menu_Y + 3 * al_get_bitmap_height(WALK_FOWARD_ACTION)+10, sensors_menu_color);
    al_draw_filled_rectangle(0, actions_menu_Y, al_get_bitmap_width(mini_menu[0])+5, actions_menu_Y + 3 * al_get_bitmap_height(WALK_FOWARD_ACTION)+10, functions_menu_color);


    //desenha todos os botões que criam blocos
    for (int i = 0; i < 6; i++) {
        al_draw_bitmap(mini_menu[i], 0, blocks_menu_Y + i * al_get_bitmap_height(mini_menu[i]), 0);
    }
    //desenha botoes de sensores
    al_draw_bitmap(BLACK_SENSOR_FUNCTION, 0, sensors_menu_Y + 0 * al_get_bitmap_height(WALK_FOWARD_ACTION) + 5, 0);
    al_draw_bitmap(COLOR_SENSOR_FUNCTION, 0, sensors_menu_Y + 1 * al_get_bitmap_height(WALK_FOWARD_ACTION) + 5, 0);
    al_draw_bitmap(ULTRASONIC_SENSOR_FUNCTION, 0, sensors_menu_Y + 2 * al_get_bitmap_height(WALK_FOWARD_ACTION) + 5, 0);
    //desenha botoes de acao
    al_draw_bitmap(WALK_FOWARD_ACTION, 0, actions_menu_Y + 0 * al_get_bitmap_height(WALK_FOWARD_ACTION) + 5, 0);
    al_draw_bitmap(TURN_LEFT_ACTION, 0, actions_menu_Y + 1 * al_get_bitmap_height(WALK_FOWARD_ACTION) + 5, 0);
    al_draw_bitmap(TURN_RIGHT_ACTION, 0, actions_menu_Y + 2 * al_get_bitmap_height(WALK_FOWARD_ACTION) + 5, 0);
    //desenha botoes extras
    al_draw_bitmap(logic_true, 0, extra_menu_Y + 0 * al_get_bitmap_height(WALK_FOWARD_ACTION) + 5, 0);
    al_draw_bitmap(logic_false, 0, extra_menu_Y + 1 * al_get_bitmap_height(WALK_FOWARD_ACTION) + 5, 0);
    al_draw_bitmap(NUMBER[2], 0, extra_menu_Y + 2 * al_get_bitmap_height(WALK_FOWARD_ACTION) + 5, 0);

    //variaveis usadas para facilitar a impressao das imagens
    int black_sensor_submenu_X = al_get_bitmap_width(mini_menu[0]) + 5;
    int black_sensor_submenu_Y = sensors_menu_Y + 0 * al_get_bitmap_height(WALK_FOWARD_ACTION)+5;
    int color_sensor_submenu_X = al_get_bitmap_width(mini_menu[0]) + 5;
    int color_sensor_submenu_Y = sensors_menu_Y + 1 * al_get_bitmap_height(WALK_FOWARD_ACTION)+5;
    int ultrasonic_sensor_submenu_X = al_get_bitmap_width(mini_menu[0]) + 5;
    int ultrasonic_sensor_submenu_Y = sensors_menu_Y + 2 * al_get_bitmap_height(WALK_FOWARD_ACTION)+5;

    //imprime submenus
    if(black_sensor_menu_selected == true) {
        sub_menu = true;
        al_draw_bitmap(BLACK_SENSOR_1_FUNCTION, black_sensor_submenu_X + 0 * al_get_bitmap_height(WALK_FOWARD_ACTION), black_sensor_submenu_Y, 0);
        al_draw_bitmap(BLACK_SENSOR_2_FUNCTION, black_sensor_submenu_X + 1 * al_get_bitmap_height(WALK_FOWARD_ACTION), black_sensor_submenu_Y, 0);
        al_draw_bitmap(BLACK_SENSOR_3_FUNCTION, black_sensor_submenu_X + 2 * al_get_bitmap_height(WALK_FOWARD_ACTION), black_sensor_submenu_Y, 0);
        al_draw_bitmap(BLACK_SENSOR_4_FUNCTION, black_sensor_submenu_X + 3 * al_get_bitmap_height(WALK_FOWARD_ACTION), black_sensor_submenu_Y, 0);
        al_draw_bitmap(BLACK_SENSOR_5_FUNCTION, black_sensor_submenu_X + 4 * al_get_bitmap_height(WALK_FOWARD_ACTION), black_sensor_submenu_Y, 0);
        for(int i=0; i<5; i++) {
            // checa se o mouse está sobre o menu de blocos e desenha um retangulo selecionando o botao que o mouse está sobre
            if((mouseX > black_sensor_submenu_X + i * al_get_bitmap_height(WALK_FOWARD_ACTION)) && (mouseX < black_sensor_submenu_X + (i+1) * al_get_bitmap_height(WALK_FOWARD_ACTION))) {
                if((mouseY > black_sensor_submenu_Y) && (mouseY < black_sensor_submenu_Y+5 + al_get_bitmap_height(mini_menu[0]))) {
                    al_draw_rectangle(black_sensor_submenu_X + i * al_get_bitmap_height(WALK_FOWARD_ACTION), black_sensor_submenu_Y, black_sensor_submenu_X + (i+1) * al_get_bitmap_height(WALK_FOWARD_ACTION), black_sensor_submenu_Y + al_get_bitmap_height(mini_menu[0]), black, 3);
                    menu_selected = 24 + i;
                }
            }
        }
    }

    if(color_sensor_menu_selected == true) {
        sub_menu = true;
        al_draw_bitmap(COLOR_SENSOR_1_FUNCTION, color_sensor_submenu_X + 0 * al_get_bitmap_height(WALK_FOWARD_ACTION), color_sensor_submenu_Y, 0);
        al_draw_bitmap(COLOR_SENSOR_2_FUNCTION, color_sensor_submenu_X + 1 * al_get_bitmap_height(WALK_FOWARD_ACTION), color_sensor_submenu_Y, 0);
        for(int i=0; i<2; i++) {
            // checa se o mouse está sobre o menu de blocos e desenha um retangulo selecionando o botao que o mouse está sobre
            if((mouseX > color_sensor_submenu_X + i * al_get_bitmap_height(WALK_FOWARD_ACTION)) && (mouseX < color_sensor_submenu_X + (i+1) * al_get_bitmap_height(WALK_FOWARD_ACTION))) {
                if((mouseY > color_sensor_submenu_Y) && (mouseY < color_sensor_submenu_Y+5 + al_get_bitmap_height(mini_menu[0]))) {
                    al_draw_rectangle(color_sensor_submenu_X + i * al_get_bitmap_height(WALK_FOWARD_ACTION), color_sensor_submenu_Y, color_sensor_submenu_X + (i+1) * al_get_bitmap_height(WALK_FOWARD_ACTION), color_sensor_submenu_Y + al_get_bitmap_height(mini_menu[0]), black, 3);
                    menu_selected = 29 + i;
                }
            }
        }
    }

    if(ultrasonic_sensor_menu_selected == true) {
        sub_menu = true;
        al_draw_bitmap(ULTRASONIC_SENSOR_1_FUNCTION, ultrasonic_sensor_submenu_X + 0 * al_get_bitmap_height(WALK_FOWARD_ACTION), ultrasonic_sensor_submenu_Y, 0);
        al_draw_bitmap(ULTRASONIC_SENSOR_2_FUNCTION, ultrasonic_sensor_submenu_X + 1 * al_get_bitmap_height(WALK_FOWARD_ACTION), ultrasonic_sensor_submenu_Y, 0);
        al_draw_bitmap(ULTRASONIC_SENSOR_3_FUNCTION, ultrasonic_sensor_submenu_X + 2 * al_get_bitmap_height(WALK_FOWARD_ACTION), ultrasonic_sensor_submenu_Y, 0);
        for(int i=0; i<3; i++) {
            // checa se o mouse está sobre o menu de blocos e desenha um retangulo selecionando o botao que o mouse está sobre
            if((mouseX > ultrasonic_sensor_submenu_X + i * al_get_bitmap_height(WALK_FOWARD_ACTION)) && (mouseX < ultrasonic_sensor_submenu_X + (i+1) * al_get_bitmap_height(WALK_FOWARD_ACTION))) {
                if((mouseY > ultrasonic_sensor_submenu_Y) && (mouseY < ultrasonic_sensor_submenu_Y+5 + al_get_bitmap_height(mini_menu[0]))) {
                    al_draw_rectangle(ultrasonic_sensor_submenu_X + i * al_get_bitmap_height(WALK_FOWARD_ACTION), ultrasonic_sensor_submenu_Y, ultrasonic_sensor_submenu_X + (i+1) * al_get_bitmap_height(WALK_FOWARD_ACTION), ultrasonic_sensor_submenu_Y + al_get_bitmap_height(mini_menu[0]), black, 3);
                    menu_selected = 31 + i;
                }
            }
        }
    }

    for(int i=0; i<6; i++) {
        // checa se o mouse está sobre o menu de blocos e desenha um retangulo selecionando o botao que o mouse está sobre
        if((mouseX > 0) && (mouseX < al_get_bitmap_width(WALK_FOWARD_ACTION))) {
            if((mouseY > al_get_bitmap_height(play_button) + 4 + i*al_get_bitmap_height(mini_menu[0])) && (mouseY < al_get_bitmap_height(play_button) + 4 + (i+1)*al_get_bitmap_height(mini_menu[0]))) {
                al_draw_rectangle(0, al_get_bitmap_height(play_button) + 4 + i*al_get_bitmap_height(mini_menu[0]), al_get_bitmap_width(mini_menu[0]), al_get_bitmap_height(play_button)+4 + (i+1)*al_get_bitmap_height(mini_menu[0]), black, 3);
                menu_selected = 9 + i;
            }
        }
    }

    for(int i=6; i<9; i++) {
        // checa se o mouse está sobre o menu de sensores e desenha um retangulo selecionando o botao que o mouse está sobre
        if((mouseX > 0) && (mouseX < al_get_bitmap_width(WALK_FOWARD_ACTION))) {
            if((mouseY > al_get_bitmap_height(play_button) + 24 + i*al_get_bitmap_height(mini_menu[0])) && (mouseY < al_get_bitmap_height(play_button) + 24 + (i+1)*al_get_bitmap_height(mini_menu[0]))) {
                al_draw_rectangle(0, al_get_bitmap_height(play_button) + 24 + i*al_get_bitmap_height(mini_menu[0]), al_get_bitmap_width(mini_menu[0]), al_get_bitmap_height(play_button)+ 24 + (i+1)*al_get_bitmap_height(mini_menu[0]), black, 3);
                menu_selected = 9 + i;
            }
        }
    }

    for(int i=9; i<12; i++) {
        // checa se o mouse está sobre o menu e desenha um retangulo selecionando o botao que o mouse está sobre
        if((mouseX > 0) && (mouseX < al_get_bitmap_width(WALK_FOWARD_ACTION))) {
            if((mouseY > al_get_bitmap_height(play_button) + 44 + i*al_get_bitmap_height(mini_menu[0])) && (mouseY < al_get_bitmap_height(play_button) + 44 + (i+1)*al_get_bitmap_height(mini_menu[0]))) {
                al_draw_rectangle(0, al_get_bitmap_height(play_button) + 44 + i*al_get_bitmap_height(mini_menu[0]), al_get_bitmap_width(mini_menu[0]), al_get_bitmap_height(play_button)+ 44 + (i+1)*al_get_bitmap_height(mini_menu[0]), black, 3);
                menu_selected = 9 + i;
            }
        }
    }

    for(int i=12; i<15; i++) {
        // checa se o mouse está sobre o menu e desenha um retangulo selecionando o botao que o mouse está sobre
        if((mouseX > 0) && (mouseX < al_get_bitmap_width(WALK_FOWARD_ACTION))) {
            if((mouseY > al_get_bitmap_height(play_button) + 64 + i*al_get_bitmap_height(mini_menu[0])) && (mouseY < al_get_bitmap_height(play_button) + 64 + (i+1)*al_get_bitmap_height(mini_menu[0]))) {
                al_draw_rectangle(0, al_get_bitmap_height(play_button) + 64 + i*al_get_bitmap_height(mini_menu[0]), al_get_bitmap_width(mini_menu[0]), al_get_bitmap_height(play_button)+ 64 + (i+1)*al_get_bitmap_height(mini_menu[0]), black, 3);
                menu_selected = 9 + i;
            }
        }
    }

    al_draw_bitmap(trash, 0, al_get_display_height(display)-70, 0);
}
void Interface :: print_function_block(Block *b) {
    if((mouseX > b->getX()) && (mouseX < (b->getX() + b->getWidth())) && (mouseY > b->getY()) && (mouseY < (b->getY() + b->getHeight()))) {
        al_draw_bitmap(FUNCTION_BLOCK[1], b->getX(), b->getY(), 0);

    } else if(b->getSelected() == true) {
        al_draw_bitmap(FUNCTION_BLOCK[2], b->getX(), b->getY(), 0);
    } else {
        al_draw_bitmap(FUNCTION_BLOCK[0], b->getX(), b->getY(), 0);
    }

    //testa qual o tipo de sensor da comparação para imprimir a imagem certa
    switch(b->getFunction()) {
        case 1:
            al_draw_bitmap(MICRO_WALK_FOWARD, b->getX()+32, b->getY()+7, 0);
            break;
        case 2:
            al_draw_bitmap(MICRO_TURN_LEFT, b->getX()+32, b->getY()+7, 0);
            break;
        case 3:
            al_draw_bitmap(MICRO_TURN_RIGHT, b->getX()+32, b->getY()+7, 0);
            break;
    }

    //desenha as bolinhas onde são ligadas as linhas
    if((mouseX > b->getX()+40) && (mouseX < (b->getX() + 53)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
        al_draw_bitmap(POINT[1], b->getX()+40, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+40, b->getY()+35, 0);
    } else if((mouseX > b->getX()+40) && (mouseX < (b->getX() + 53)) && (mouseY > b->getY()+35) && (mouseY < (b->getY()+48))) {
        al_draw_bitmap(POINT[0], b->getX()+40, b->getY()-5, 0);
        al_draw_bitmap(POINT[1], b->getX()+40, b->getY()+35, 0);
    } else {
        al_draw_bitmap(POINT[0], b->getX()+40, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+40, b->getY()+35, 0);
    }
}
void Interface :: print_end_block(Block *b) {
    if((mouseX > b->getX()) && (mouseX < (b->getX() + b->getWidth())) && (mouseY > b->getY()) && (mouseY < (b->getY() + b->getHeight()))) {
        al_draw_bitmap(END_BLOCK[1], b->getX(), b->getY(), 0);
    } else if(b->getSelected() == true) {
        al_draw_bitmap(END_BLOCK[2], b->getX(), b->getY(), 0);
    } else {
        al_draw_bitmap(END_BLOCK[0], b->getX(), b->getY(), 0);
    }
    //desenha as bolinhas onde são ligadas as linhas
    if((mouseX > b->getX()+37) && (mouseX < (b->getX() + 50)) && (mouseY > b->getY()-5) && (mouseY < (b->getY() + 8))) {
        al_draw_bitmap(POINT[1], b->getX()+37, b->getY()-5, 0);
    } else {
        al_draw_bitmap(POINT[0], b->getX()+37, b->getY()-5, 0);
    }
}
void Interface :: print_start_block(Block *b) {
    if((mouseX > b->getX()) && (mouseX < (b->getX() + b->getWidth())) && (mouseY > b->getY()) && (mouseY < (b->getY() + b->getHeight()))) {
        al_draw_bitmap(START_BLOCK[1], b->getX(), b->getY(), 0);
    } else if(b->getSelected() == true) {
        al_draw_bitmap(START_BLOCK[2], b->getX(), b->getY(), 0);
    } else {
        al_draw_bitmap(START_BLOCK[0], b->getX(), b->getY(), 0);
    }
    //desenha as bolinhas onde são ligadas as linhas
    if((mouseX > b->getX()+37) && (mouseX < (b->getX() + 50)) && (mouseY > b->getY()+27) && (mouseY < (b->getY() + 40))) {
        al_draw_bitmap(POINT[1], b->getX()+37, b->getY()+27, 0);
    } else {
        al_draw_bitmap(POINT[0], b->getX()+37, b->getY()+27, 0);
    }
}
void Interface :: print_loop_block(Block *b) {
    if((mouseX > b->getX()) && (mouseX < (b->getX() + b->getWidth())) && (mouseY > b->getY()) && (mouseY < (b->getY() + b->getHeight()))) {
        al_draw_bitmap(LOOP_BLOCK[1], b->getX(), b->getY(), 0);
    } else if(b->getSelected() == true) {
        al_draw_bitmap(LOOP_BLOCK[2], b->getX(), b->getY(), 0);
    } else {
        al_draw_bitmap(LOOP_BLOCK[0], b->getX(), b->getY(), 0);
    }
    //desenha as bolinhas onde são ligadas as linhas
    if((mouseX > b->getX()+35) && (mouseX < (b->getX() + 48)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
        al_draw_bitmap(POINT[1], b->getX()+35, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+35, b->getY()+81, 0);
        al_draw_bitmap(POINT[0], b->getX()+72, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+72, b->getY()+81, 0);
    } else if((mouseX > b->getX()+35) && (mouseX < (b->getX() + 48)) && (mouseY > b->getY()+81) && (mouseY < (b->getY()+94))) {
        al_draw_bitmap(POINT[0], b->getX()+35, b->getY()-5, 0);
        al_draw_bitmap(POINT[1], b->getX()+35, b->getY()+81, 0);
        al_draw_bitmap(POINT[0], b->getX()+72, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+72, b->getY()+81, 0);
    } else if((mouseX > b->getX()+72) && (mouseX < (b->getX() + 85)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
        al_draw_bitmap(POINT[0], b->getX()+35, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+35, b->getY()+81, 0);
        al_draw_bitmap(POINT[1], b->getX()+72, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+72, b->getY()+81, 0);
    } else if((mouseX > b->getX()+72) && (mouseX < (b->getX() + 85)) && (mouseY > b->getY()+81) && (mouseY < (b->getY()+94))) {
        al_draw_bitmap(POINT[0], b->getX()+35, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+35, b->getY()+81, 0);
        al_draw_bitmap(POINT[0], b->getX()+72, b->getY()-5, 0);
        al_draw_bitmap(POINT[1], b->getX()+72, b->getY()+81, 0);
    } else {
        al_draw_bitmap(POINT[0], b->getX()+35, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+35, b->getY()+81, 0);
        al_draw_bitmap(POINT[0], b->getX()+72, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+72, b->getY()+81, 0);
    }
}
void Interface :: print_decision_block(Block *b) {

    //desenha o bloco
    if((mouseX > b->getX()) && (mouseX < (b->getX() + b->getWidth())) && (mouseY > b->getY()) && (mouseY < (b->getY() + b->getHeight()))) {
        al_draw_bitmap(DECISION_BLOCK[1], b->getX(), b->getY(), 0);
    } else if(b->getSelected() == true) {
        al_draw_bitmap(DECISION_BLOCK[2], b->getX(), b->getY(), 0);
    } else {
        al_draw_bitmap(DECISION_BLOCK[0], b->getX(), b->getY(), 0);
    }
    //testa qual o tipo de sensor da comparação para imprimir a imagem certa
    switch(b->getTypeOfSensor()) {
        case 1:
            al_draw_bitmap(MICRO_BLACK_SENSOR1, b->getX()+47, b->getY()+19, 0);
            break;

        case 2:
            al_draw_bitmap(MICRO_BLACK_SENSOR2, b->getX()+47, b->getY()+19, 0);
            break;
        case 3:
            al_draw_bitmap(MICRO_BLACK_SENSOR3, b->getX()+47, b->getY()+19, 0);
            break;
        case 4:
            al_draw_bitmap(MICRO_BLACK_SENSOR4, b->getX()+47, b->getY()+19, 0);
            break;
        case 5:
            al_draw_bitmap(MICRO_BLACK_SENSOR5, b->getX()+47, b->getY()+19, 0);
            break;
        case 6:
            al_draw_bitmap(MICRO_COLOR_SENSOR1, b->getX()+47, b->getY()+19, 0);
            break;
        case 7:
            al_draw_bitmap(MICRO_COLOR_SENSOR2, b->getX()+47, b->getY()+19, 0);
            break;
        case 8:
            al_draw_bitmap(MICRO_ULTRASONIC_SENSOR1, b->getX()+47, b->getY()+19, 0);
            break;
        case 9:
            al_draw_bitmap(MICRO_ULTRASONIC_SENSOR2, b->getX()+47, b->getY()+19, 0);
            break;
        case 10:
            al_draw_bitmap(MICRO_ULTRASONIC_SENSOR3, b->getX()+47, b->getY()+19, 0);
            break;
    }

    //desenha as bolinhas onde são ligadas as linhas
    if((mouseX > b->getX()+53) && (mouseX < (b->getX() + 66)) && (mouseY > b->getY()-6) && (mouseY < (b->getY()+7))) {
        al_draw_bitmap(POINT[1], b->getX()+53, b->getY()-6, 0);
        al_draw_bitmap(POINT[0], b->getX()+53, b->getY()+62, 0);
        al_draw_bitmap(POINT[0], b->getX()+112, b->getY()+28, 0);
    } else if((mouseX > b->getX()+53) && (mouseX < (b->getX() + 66)) && (mouseY > b->getY()+62) && (mouseY < (b->getY()+75))) {
        al_draw_bitmap(POINT[0], b->getX()+53, b->getY()-6, 0);
        al_draw_bitmap(POINT[1], b->getX()+53, b->getY()+62, 0);
        al_draw_bitmap(POINT[0], b->getX()+112, b->getY()+28, 0);
    } else if((mouseX > b->getX()+112) && (mouseX < (b->getX() + 125)) && (mouseY > b->getY()+28) && (mouseY < (b->getY()+41))) {
        al_draw_bitmap(POINT[0], b->getX()+53, b->getY()-6, 0);
        al_draw_bitmap(POINT[0], b->getX()+53, b->getY()+62, 0);
        al_draw_bitmap(POINT[1], b->getX()+112, b->getY()+28, 0);
    } else {
        al_draw_bitmap(POINT[0], b->getX()+53, b->getY()-6, 0);
        al_draw_bitmap(POINT[0], b->getX()+53, b->getY()+62, 0);
        al_draw_bitmap(POINT[0], b->getX()+112, b->getY()+28, 0);
    }
}
void Interface :: print_merge_block(Block *b) {
    if((mouseX > b->getX()) && (mouseX < (b->getX() + b->getWidth())) && (mouseY > b->getY()) && (mouseY < (b->getY() + b->getHeight()))) {
        al_draw_bitmap(MERGE_BLOCK[1], b->getX(), b->getY(), 0);
    } else if(b->getSelected() == true) {
        al_draw_bitmap(MERGE_BLOCK[2], b->getX(), b->getY(), 0);
    } else {
        al_draw_bitmap(MERGE_BLOCK[0], b->getX(), b->getY(), 0);
    }
    //desenha as bolinhas onde são ligadas as linhas
    if((mouseX > b->getX()-5) && (mouseX < (b->getX() + 8)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
        al_draw_bitmap(POINT[1], b->getX()-5, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+22, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+8, b->getY()+17, 0);
    } else if((mouseX > b->getX()+22) && (mouseX < (b->getX() + 35)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
        al_draw_bitmap(POINT[0], b->getX()-5, b->getY()-5, 0);
        al_draw_bitmap(POINT[1], b->getX()+22, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+8, b->getY()+17, 0);
    } else if((mouseX > b->getX()+8) && (mouseX < (b->getX() + 21)) && (mouseY > b->getY()+17) && (mouseY < (b->getY()+30))) {
        al_draw_bitmap(POINT[0], b->getX()-5, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+22, b->getY()-5, 0);
        al_draw_bitmap(POINT[1], b->getX()+8, b->getY()+17, 0);
    } else {
        al_draw_bitmap(POINT[0], b->getX()-5, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+22, b->getY()-5, 0);
        al_draw_bitmap(POINT[0], b->getX()+8, b->getY()+17, 0);
    }
}
void Interface :: load_program_images() {
    //load_bitmap(&menu, "images/menu.png");
    load_bitmap(&play_button, "images/menu_buttons/play_button.png");
    load_bitmap(&play_button_selected, "images/menu_buttons/play_button_selected.png");
    load_bitmap(&pause_button, "images/menu_buttons/pause_button.png");
    load_bitmap(&pause_button_selected, "images/menu_buttons/pause_button_selected.png");
    load_bitmap(&stop_button, "images/menu_buttons/stop_button.png");
    load_bitmap(&stop_button_selected, "images/menu_buttons/stop_button_selected.png");
    load_bitmap(&save_button, "images/menu_buttons/save_button.png");
    load_bitmap(&save_button_selected, "images/menu_buttons/save_button_selected.png");
    load_bitmap(&load_button, "images/menu_buttons/load_button.png");
    load_bitmap(&load_button_selected, "images/menu_buttons/load_button_selected.png");
    load_bitmap(&save_as_button, "images/menu_buttons/save_as_button.png");
    load_bitmap(&save_as_button_selected, "images/menu_buttons/save_as_button_selected.png");
    load_bitmap(&vrep_button, "images/menu_buttons/vrep_button.png");
    load_bitmap(&vrep_button_selected, "images/menu_buttons/vrep_button_selected.png");
    load_bitmap(&bluetooth_button, "images/menu_buttons/bluetooth_button.png");
    load_bitmap(&bluetooth_button_selected, "images/menu_buttons/bluetooth_button_selected.png");

    load_bitmap(&mini_menu[0], "images/blocks/decision_block/mini_decision_block.png");
    load_bitmap(&mini_menu[1], "images/blocks/function_block/mini_function_block.png");
    load_bitmap(&mini_menu[2], "images/blocks/start_block/mini_start_block.png");
    load_bitmap(&mini_menu[3], "images/blocks/end_block/mini_end_block.png");
    load_bitmap(&mini_menu[4], "images/blocks/merge_block/mini_merge_block.png");
    load_bitmap(&mini_menu[5], "images/blocks/loop_block/mini_loop_block.png");
    load_bitmap(&WALK_FOWARD_ACTION, "images/functions/mini_action_walk_foward.png");
    load_bitmap(&MICRO_WALK_FOWARD, "images/functions/micro_action_walk_foward.png");
    load_bitmap(&TURN_LEFT_ACTION, "images/functions/mini_action_turn_left.png");
    load_bitmap(&MICRO_TURN_LEFT, "images/functions/micro_action_turn_left.png");
    load_bitmap(&TURN_RIGHT_ACTION, "images/functions/mini_action_turn_right.png");
    load_bitmap(&MICRO_TURN_RIGHT, "images/functions/micro_action_turn_right.png");
    load_bitmap(&logic_true, "images/functions/mini_logic_true.png");
    load_bitmap(&logic_false, "images/functions/mini_logic_false.png");
    load_bitmap(&POINT[0], "images/point.png");
    load_bitmap(&POINT[1], "images/hpoint.png");

    load_bitmap(&DECISION_BLOCK[0], "images/blocks/decision_block/decision_block.png");
    load_bitmap(&DECISION_BLOCK[1], "images/blocks/decision_block/decision_block_mouse.png");
    load_bitmap(&DECISION_BLOCK[2], "images/blocks/decision_block/decision_block_selected.png");
    load_bitmap(&DECISION_BLOCK[3], "images/blocks/decision_block/decision_block_executing.png");
    load_bitmap(&END_BLOCK[0], "images/blocks/end_block/end_block.png");
    load_bitmap(&END_BLOCK[1], "images/blocks/end_block/end_block_mouse.png");
    load_bitmap(&END_BLOCK[2], "images/blocks/end_block/end_block_selected.png");
    load_bitmap(&END_BLOCK[3], "images/blocks/end_block/end_block_executing.png");
    load_bitmap(&FUNCTION_BLOCK[0], "images/blocks/function_block/function_block.png");
    load_bitmap(&FUNCTION_BLOCK[1], "images/blocks/function_block/function_block_mouse.png");
    load_bitmap(&FUNCTION_BLOCK[2], "images/blocks/function_block/function_block_selected.png");
    load_bitmap(&FUNCTION_BLOCK[3], "images/blocks/function_block/function_block_executing.png");
    load_bitmap(&LOOP_BLOCK[0], "images/blocks/loop_block/loop_block.png");
    load_bitmap(&LOOP_BLOCK[1], "images/blocks/loop_block/loop_block_mouse.png");
    load_bitmap(&LOOP_BLOCK[2], "images/blocks/loop_block/loop_block_selected.png");
    load_bitmap(&LOOP_BLOCK[3], "images/blocks/loop_block/loop_block_executing.png");
    load_bitmap(&MERGE_BLOCK[0], "images/blocks/merge_block/merge_block.png");
    load_bitmap(&MERGE_BLOCK[1], "images/blocks/merge_block/merge_block_mouse.png");
    load_bitmap(&MERGE_BLOCK[2], "images/blocks/merge_block/merge_block_selected.png");
    load_bitmap(&MERGE_BLOCK[3], "images/blocks/merge_block/merge_block_executing.png");
    load_bitmap(&START_BLOCK[0], "images/blocks/start_block/start_block.png");
    load_bitmap(&START_BLOCK[1], "images/blocks/start_block/start_block_mouse.png");
    load_bitmap(&START_BLOCK[2], "images/blocks/start_block/start_block_selected.png");
    load_bitmap(&START_BLOCK[3], "images/blocks/start_block/start_block_executing.png");
    load_bitmap(&BLACK_SENSOR_FUNCTION, "images/blocks/black_sensor_block/mini_black_sensor.png");
    load_bitmap(&BLACK_SENSOR_1_FUNCTION, "images/blocks/black_sensor_block/mini_black_sensor1.png");
    load_bitmap(&BLACK_SENSOR_2_FUNCTION, "images/blocks/black_sensor_block/mini_black_sensor2.png");
    load_bitmap(&BLACK_SENSOR_3_FUNCTION, "images/blocks/black_sensor_block/mini_black_sensor3.png");
    load_bitmap(&BLACK_SENSOR_4_FUNCTION, "images/blocks/black_sensor_block/mini_black_sensor4.png");
    load_bitmap(&BLACK_SENSOR_5_FUNCTION, "images/blocks/black_sensor_block/mini_black_sensor5.png");
    load_bitmap(&MICRO_BLACK_SENSOR1, "images/blocks/black_sensor_block/micro_black_sensor1.png");
    load_bitmap(&MICRO_BLACK_SENSOR2, "images/blocks/black_sensor_block/micro_black_sensor2.png");
    load_bitmap(&MICRO_BLACK_SENSOR3, "images/blocks/black_sensor_block/micro_black_sensor3.png");
    load_bitmap(&MICRO_BLACK_SENSOR4, "images/blocks/black_sensor_block/micro_black_sensor4.png");
    load_bitmap(&MICRO_BLACK_SENSOR5, "images/blocks/black_sensor_block/micro_black_sensor5.png");
    load_bitmap(&COLOR_SENSOR_FUNCTION, "images/blocks/color_sensor_block/mini_color_sensor.png");
    load_bitmap(&COLOR_SENSOR_1_FUNCTION, "images/blocks/color_sensor_block/mini_color_sensor1.png");
    load_bitmap(&COLOR_SENSOR_2_FUNCTION, "images/blocks/color_sensor_block/mini_color_sensor2.png");
    load_bitmap(&MICRO_COLOR_SENSOR1, "images/blocks/color_sensor_block/micro_color_sensor1.png");
    load_bitmap(&MICRO_COLOR_SENSOR2, "images/blocks/color_sensor_block/micro_color_sensor2.png");
    load_bitmap(&ULTRASONIC_SENSOR_FUNCTION, "images/blocks/ultrasonic_sensor_block/mini_ultrasonic_sensor.png");
    load_bitmap(&ULTRASONIC_SENSOR_1_FUNCTION, "images/blocks/ultrasonic_sensor_block/mini_ultrasonic_sensor1.png");
    load_bitmap(&ULTRASONIC_SENSOR_2_FUNCTION, "images/blocks/ultrasonic_sensor_block/mini_ultrasonic_sensor2.png");
    load_bitmap(&ULTRASONIC_SENSOR_3_FUNCTION, "images/blocks/ultrasonic_sensor_block/mini_ultrasonic_sensor3.png");
    load_bitmap(&MICRO_ULTRASONIC_SENSOR1, "images/blocks/ultrasonic_sensor_block/micro_ultrasonic_sensor1.png");
    load_bitmap(&MICRO_ULTRASONIC_SENSOR2, "images/blocks/ultrasonic_sensor_block/micro_ultrasonic_sensor2.png");
    load_bitmap(&MICRO_ULTRASONIC_SENSOR3, "images/blocks/ultrasonic_sensor_block/micro_ultrasonic_sensor3.png");

    load_bitmap(&NUMBER[0], "images/functions/mini_number_0.png");
    load_bitmap(&NUMBER[1], "images/functions/mini_number_1.png");
    load_bitmap(&NUMBER[2], "images/functions/mini_number_2.png");
    load_bitmap(&NUMBER[3], "images/functions/mini_number_3.png");
    load_bitmap(&NUMBER[4], "images/functions/mini_number_4.png");
    load_bitmap(&NUMBER[5], "images/functions/mini_number_5.png");
    load_bitmap(&NUMBER[6], "images/functions/mini_number_6.png");
    load_bitmap(&NUMBER[7], "images/functions/mini_number_7.png");
    load_bitmap(&NUMBER[8], "images/functions/mini_number_8.png");
    load_bitmap(&NUMBER[9], "images/functions/mini_number_9.png");

    load_bitmap(&trash, "images/trash_icon.png");
}
void Interface :: destroy_program_images() {
    al_destroy_bitmap(play_button);
    al_destroy_bitmap(play_button_selected);
	al_destroy_bitmap(pause_button);
    al_destroy_bitmap(pause_button_selected);
	al_destroy_bitmap(stop_button);
    al_destroy_bitmap(stop_button_selected);
	al_destroy_bitmap(save_button);
    al_destroy_bitmap(save_button_selected);
    al_destroy_bitmap(save_as_button);
    al_destroy_bitmap(save_as_button_selected);
    al_destroy_bitmap(load_button);
    al_destroy_bitmap(load_button_selected);
    al_destroy_bitmap(vrep_button);
    al_destroy_bitmap(vrep_button_selected);
	al_destroy_bitmap(bluetooth_button);
    al_destroy_bitmap(bluetooth_button_selected);
    for(int i=0; i<6; i++) {
        al_destroy_bitmap(mini_menu[i]);
    }
    al_destroy_bitmap(WALK_FOWARD_ACTION);
    al_destroy_bitmap(MICRO_WALK_FOWARD);
    al_destroy_bitmap(TURN_LEFT_ACTION);
    al_destroy_bitmap(MICRO_TURN_LEFT);
    al_destroy_bitmap(TURN_RIGHT_ACTION);
    al_destroy_bitmap(MICRO_TURN_RIGHT);
    al_destroy_bitmap(logic_true);
    al_destroy_bitmap(logic_false);
    al_destroy_bitmap(POINT[0]);
	al_destroy_bitmap(POINT[1]);
    al_destroy_bitmap(BLACK_SENSOR_FUNCTION);
    al_destroy_bitmap(BLACK_SENSOR_1_FUNCTION);
    al_destroy_bitmap(BLACK_SENSOR_2_FUNCTION);
    al_destroy_bitmap(BLACK_SENSOR_3_FUNCTION);
    al_destroy_bitmap(BLACK_SENSOR_4_FUNCTION);
    al_destroy_bitmap(BLACK_SENSOR_5_FUNCTION);
    al_destroy_bitmap(MICRO_BLACK_SENSOR1);
    al_destroy_bitmap(MICRO_BLACK_SENSOR2);
    al_destroy_bitmap(MICRO_BLACK_SENSOR3);
    al_destroy_bitmap(MICRO_BLACK_SENSOR4);
    al_destroy_bitmap(MICRO_BLACK_SENSOR5);
    al_destroy_bitmap(COLOR_SENSOR_FUNCTION);
    al_destroy_bitmap(COLOR_SENSOR_1_FUNCTION);
    al_destroy_bitmap(COLOR_SENSOR_2_FUNCTION);
    al_destroy_bitmap(MICRO_COLOR_SENSOR1);
    al_destroy_bitmap(MICRO_COLOR_SENSOR2);
    al_destroy_bitmap(ULTRASONIC_SENSOR_FUNCTION);
    al_destroy_bitmap(ULTRASONIC_SENSOR_1_FUNCTION);
    al_destroy_bitmap(ULTRASONIC_SENSOR_2_FUNCTION);
    al_destroy_bitmap(ULTRASONIC_SENSOR_3_FUNCTION);
    al_destroy_bitmap(MICRO_ULTRASONIC_SENSOR1);
    al_destroy_bitmap(MICRO_ULTRASONIC_SENSOR2);
    al_destroy_bitmap(MICRO_ULTRASONIC_SENSOR3);

    al_destroy_bitmap(trash);
    for(int i=0; i<3; i++) {
        al_destroy_bitmap(DECISION_BLOCK[i]);
    }
    for(int i=0; i<3; i++) {
        al_destroy_bitmap(END_BLOCK[i]);
    }
    for(int i=0; i<3; i++) {
        al_destroy_bitmap(FUNCTION_BLOCK[i]);
    }
    for(int i=0; i<3; i++) {
        al_destroy_bitmap(LOOP_BLOCK[i]);
    }
    for(int i=0; i<3; i++) {
        al_destroy_bitmap(MERGE_BLOCK[i]);
    }
    for(int i=0; i<3; i++) {
        al_destroy_bitmap(START_BLOCK[i]);
    }
    for(int i=0; i<10; i++) {
        al_destroy_bitmap(NUMBER[i]);
    }
}
void Interface :: reset_dragging_variables() {
    dragging_black_sensor1 = false;
    dragging_black_sensor2 = false;
    dragging_black_sensor3 = false;
    dragging_black_sensor4 = false;
    dragging_black_sensor5 = false;
    dragging_color_sensor1 = false;
    dragging_color_sensor2 = false;
    dragging_ultrasonic_sensor1 = false;
    dragging_ultrasonic_sensor2 = false;
    dragging_ultrasonic_sensor3 = false;
    dragging_walk_foward = false;
    dragging_turn_left = false;
    dragging_turn_right = false;
}
void Interface :: check_dragging() {

    for(int i=0; i<valor_maximo_blocos; i++) {
        if(blocks_list_to_print[i] != NULL) {
            if(blocks_list_to_print[i]->getType() == 8) {
                if((mouseX > blocks_list_to_print[i]->getX()) && (mouseX < (blocks_list_to_print[i]->getX() + blocks_list_to_print[i]->getWidth())) && (mouseY > blocks_list_to_print[i]->getY()) && (mouseY < (blocks_list_to_print[i]->getY() + blocks_list_to_print[i]->getHeight()))) {
                    //cout<<"soltou sobre o bloco certo"<<endl;
                    if(dragging_black_sensor1 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(1);
                    } else if(dragging_black_sensor2 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(2);
                    } else if(dragging_black_sensor3 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(3);
                    } else if(dragging_black_sensor4 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(4);
                    } else if(dragging_black_sensor5 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(5);
                    } else if(dragging_color_sensor1 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(6);
                    } else if(dragging_color_sensor2 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(7);
                    } else if(dragging_ultrasonic_sensor1 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(8);
                    } else if(dragging_ultrasonic_sensor2 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(9);
                    } else if(dragging_ultrasonic_sensor3 == true) {
                        blocks_list_to_print[i]->setTypeOfSensor(10);
                    } else if((dragging_walk_foward == true) || (dragging_turn_left == true) || (dragging_turn_right == true)) {
                        //erro
                        al_show_native_message_box(NULL, NULL, NULL, "Essa ação não pode ser usado como entrada no bloco de decisão", NULL, NULL);
                    }
                }
            }
            if(blocks_list_to_print[i]->getType() == 1) {
                if((mouseX > blocks_list_to_print[i]->getX()) && (mouseX < (blocks_list_to_print[i]->getX() + blocks_list_to_print[i]->getWidth())) && (mouseY > blocks_list_to_print[i]->getY()) && (mouseY < (blocks_list_to_print[i]->getY() + blocks_list_to_print[i]->getHeight()))) {
                    //cout<<"soltou sobre o bloco certo"<<endl;
                    if((dragging_black_sensor1 == true) || (dragging_black_sensor2 == true) || (dragging_black_sensor3 == true)  || (dragging_black_sensor4 == true) || (dragging_black_sensor5 == true)){
                        //erro
                        al_show_native_message_box(NULL, NULL, NULL, "O sensor de fita preta não pode ser usado como entrada no bloco de função", NULL, NULL);
                    }  else if((dragging_color_sensor1 == true) || (dragging_color_sensor2 == true)){
                        //erro
                        al_show_native_message_box(NULL, NULL, NULL, "O sensor de cor não pode ser usado como entrada no bloco de função", NULL, NULL);
                    } else if((dragging_ultrasonic_sensor1 == true) || (dragging_ultrasonic_sensor2 == true) || (dragging_ultrasonic_sensor3 == true)) {
                        //erro
                        al_show_native_message_box(NULL, NULL, NULL, "O sensor de ultrassom não pode ser usado como entrada no bloco de função", NULL, NULL);
                    } else if(dragging_walk_foward == true) {
                        blocks_list_to_print[i]->setFunction(1);
                    } else if(dragging_turn_left == true) {
                        blocks_list_to_print[i]->setFunction(2);
                    } else if(dragging_turn_right == true) {
                        blocks_list_to_print[i]->setFunction(3);
                    }
                }
            }
        }
    }
}
void Interface :: draw_dragging() {
    if(dragging_black_sensor1 == true) {
        al_draw_bitmap(BLACK_SENSOR_1_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_black_sensor2 == true) {
        al_draw_bitmap(BLACK_SENSOR_2_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_black_sensor3 == true) {
        al_draw_bitmap(BLACK_SENSOR_3_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_black_sensor4 == true) {
        al_draw_bitmap(BLACK_SENSOR_4_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_black_sensor5 == true) {
        al_draw_bitmap(BLACK_SENSOR_5_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_color_sensor1 == true) {
        al_draw_bitmap(COLOR_SENSOR_1_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_color_sensor2 == true) {
        al_draw_bitmap(COLOR_SENSOR_2_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_ultrasonic_sensor1 == true) {
        al_draw_bitmap(ULTRASONIC_SENSOR_1_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_ultrasonic_sensor2 == true) {
        al_draw_bitmap(ULTRASONIC_SENSOR_2_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_ultrasonic_sensor3 == true) {
        al_draw_bitmap(ULTRASONIC_SENSOR_3_FUNCTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_walk_foward == true) {
        al_draw_bitmap(WALK_FOWARD_ACTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_turn_left == true) {
        al_draw_bitmap(TURN_LEFT_ACTION, mouseX-20, mouseY-20, 0);
    }
    if(dragging_turn_right == true) {
        al_draw_bitmap(TURN_RIGHT_ACTION, mouseX-20, mouseY-20, 0);
    }
}
void Interface :: check_mouse_on_menus() {
    menu1_X_limit = 14 + 6*al_get_bitmap_width(play_button);
    menu1_Y_limit = 4 + al_get_bitmap_height(play_button);

    menu_connect_X_begin = al_get_display_width(display) - 73.5 - al_get_bitmap_width(bluetooth_button) - roll_bar_width;
    menu_connect_X_limit = al_get_display_width(display) - roll_bar_width;

    menu2_X_limit = al_get_bitmap_width(WALK_FOWARD_ACTION);

    menu_blocks_Y_begin = 4 + al_get_bitmap_height(play_button);
    menu_blocks_Y_limit = menu1_Y_limit + 6*al_get_bitmap_height(WALK_FOWARD_ACTION);

    menu_sensors_Y_begin = menu_blocks_Y_limit + 20;
    menu_sensors_Y_limit = menu_sensors_Y_begin + 3*al_get_bitmap_height(WALK_FOWARD_ACTION);

    menu_actions_Y_begin = menu_sensors_Y_limit + 20;
    menu_actions_Y_limit = menu_actions_Y_begin + 3*al_get_bitmap_height(WALK_FOWARD_ACTION);

    menu_extra_Y_begin = menu_actions_Y_limit + 20;
    menu_extra_Y_limit = menu_extra_Y_begin + 3*al_get_bitmap_height(WALK_FOWARD_ACTION);

    //se o mouse não está sobre os menus seta a variável de teste para zero
    if(mouseY < menu1_Y_limit) {
        if((mouseX > 14) && (mouseX < menu1_X_limit)) {
            //menu 1
        } else if((mouseX < menu_connect_X_limit) && (mouseX > menu_connect_X_begin)) {
            //menu conectar
        } else {
            menu_selected = 0;
        }
    } else if(mouseX < menu2_X_limit) {
        if((mouseY > menu_blocks_Y_begin) && (mouseY < menu_blocks_Y_limit)) {
            //menu de blocos
        } else if((mouseY > menu_sensors_Y_begin) && (mouseY < menu_sensors_Y_limit)) {
            //menu de sensores
        } else if((mouseY > menu_actions_Y_begin) && (mouseY < menu_actions_Y_limit)) {
            //menu de acoes
        } else if((mouseY > menu_extra_Y_begin) && (mouseY < menu_extra_Y_limit)) {
            //menu extra
        } else {
            menu_selected = 0;
        }
    } else if((black_sensor_menu_selected == true) && ((mouseX < 6*menu2_X_limit) && (mouseY > menu_sensors_Y_begin) && (mouseY < (menu_sensors_Y_begin+al_get_bitmap_height(WALK_FOWARD_ACTION))))) {
        //submenu sensor de fita
    } else if((color_sensor_menu_selected == true) && ((mouseX < 3*menu2_X_limit) && (mouseY > (menu_sensors_Y_begin+al_get_bitmap_width(WALK_FOWARD_ACTION))) && (mouseY < (menu_sensors_Y_begin+2*al_get_bitmap_height(WALK_FOWARD_ACTION))))) {
        //submenu sensor de cor
    } else if((ultrasonic_sensor_menu_selected == true) && ((mouseX < 4*menu2_X_limit) && (mouseY > (menu_sensors_Y_begin+2*al_get_bitmap_width(WALK_FOWARD_ACTION))) && (mouseY < (menu_sensors_Y_begin+3*al_get_bitmap_height(WALK_FOWARD_ACTION))))) {
        //submenu sensor de ultrassom
    } else {
        menu_selected = 0;
    }
}
void Interface :: print_list_of_blocks() {
    for(int i=0; i<valor_maximo_blocos; i++) {
        //testa se na posição do array existe mesmo um bloco
        if(blocks_list_to_print[i] != NULL) {
            /*
                tipo 1 = bloco de ação
                tipo 2 = bloco de sensor de ultrassom
                tipo 3 = bloco de sensor de fita preta
                tipo 4 = bloco de sensor de cor
                tipo 5 = bloco de fim
                tipo 6 = bloco de início
                tipo 7 = bloco de loop
                tipo 8 = bloco de decisão
                tipo 9 = bloco de junção
            */
            if(blocks_list_to_print[i]->getType() == 1) {
                print_function_block(blocks_list_to_print[i]);
            }
            if(blocks_list_to_print[i]->getType() == 2) {

            }
            if(blocks_list_to_print[i]->getType() == 3) {

            }
            if(blocks_list_to_print[i]->getType() == 4) {

            }
            if(blocks_list_to_print[i]->getType() == 5) {
                print_end_block(blocks_list_to_print[i]);
            }
            if(blocks_list_to_print[i]->getType() == 6) {
                print_start_block(blocks_list_to_print[i]);
            }
            if(blocks_list_to_print[i]->getType() == 7) {
                print_loop_block(blocks_list_to_print[i]);
            }
            if(blocks_list_to_print[i]->getType() == 8) {
                print_decision_block(blocks_list_to_print[i]);
            }
            if(blocks_list_to_print[i]->getType() == 9) {
                print_merge_block(blocks_list_to_print[i]);
            }
            //se o block estiver sendo arrastado, atualiza as cordenadas
            if(blocks_list_to_print[i]->getDragging() == true) {
                blocks_list_to_print[i]->setX(mouseX - mouse_aux_x);
                blocks_list_to_print[i]->setY(mouseY - mouse_aux_y);
            }
        }
    }
}
void Interface :: check_mouse_on_points(Block *b) {
    //função
    if(b->getType() == 1) {
        if((mouseX > b->getX()+40) && (mouseX < (b->getX() + 53)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
            cout<<"clicou na entrada do bloco de funcao"<<endl;
            b->setIn1Selected(true);
            b->setOut1Selected(false);
        } else if((mouseX > b->getX()+40) && (mouseX < (b->getX() + 53)) && (mouseY > b->getY()+35) && (mouseY < (b->getY()+48))) {
            cout<<"clicou na saida do bloco de funcao"<<endl;
            b->setIn1Selected(false);
            b->setOut1Selected(true);
        } else {
            b->setIn1Selected(false);
            b->setOut1Selected(false);
        }
    }
    //fim
    if(b->getType() == 5) {
        if((mouseX > b->getX()+37) && (mouseX < (b->getX() + 50)) && (mouseY > b->getY()-5) && (mouseY < (b->getY() + 8))) {
            cout<<"clicou na entrada do bloco de fim"<<endl;
            b->setIn1Selected(true);
        } else {
            b->setIn1Selected(false);
        }
    }
    //inicio
    if(b->getType() == 6) {
        if((mouseX > b->getX()+37) && (mouseX < (b->getX() + 50)) && (mouseY > b->getY()+27) && (mouseY < (b->getY() + 40))) {
            b->setOut1Selected(true);
        } else {
            b->setOut1Selected(false);
        }
    }
    //loop
    if(b->getType() == 7) {
        if((mouseX > b->getX()+35) && (mouseX < (b->getX() + 48)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
            b->setIn1Selected(true);
            b->setOut1Selected(false);
            b->setIn2Selected(false);
            b->setOut2Selected(false);
        } else if((mouseX > b->getX()+35) && (mouseX < (b->getX() + 48)) && (mouseY > b->getY()+81) && (mouseY < (b->getY()+94))) {
            b->setIn1Selected(false);
            b->setOut1Selected(true);
            b->setIn2Selected(false);
            b->setOut2Selected(false);
        } else if((mouseX > b->getX()+72) && (mouseX < (b->getX() + 85)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
            b->setIn1Selected(false);
            b->setOut1Selected(false);
            b->setIn2Selected(true);
            b->setOut2Selected(false);
        } else if((mouseX > b->getX()+72) && (mouseX < (b->getX() + 85)) && (mouseY > b->getY()+81) && (mouseY < (b->getY()+94))) {
            b->setIn1Selected(false);
            b->setOut1Selected(false);
            b->setIn2Selected(false);
            b->setOut2Selected(true);
        } else {
            b->setIn1Selected(false);
            b->setOut1Selected(false);
            b->setIn2Selected(false);
            b->setOut2Selected(false);
        }
    }
    //decisao
    if(b->getType() == 8) {

        if((mouseX > b->getX()+53) && (mouseX < (b->getX() + 66)) && (mouseY > b->getY()-6) && (mouseY < (b->getY()+7))) {
            b->setIn1Selected(true);
            b->setOut1Selected(false);
            b->setOut2Selected(false);
        } else if((mouseX > b->getX()+53) && (mouseX < (b->getX() + 66)) && (mouseY > b->getY()+62) && (mouseY < (b->getY()+75))) {
            b->setIn1Selected(false);
            b->setOut1Selected(false);
            b->setOut2Selected(true);
        } else if((mouseX > b->getX()+112) && (mouseX < (b->getX() + 125)) && (mouseY > b->getY()+28) && (mouseY < (b->getY()+41))) {
            b->setIn1Selected(false);
            b->setOut1Selected(true);
            b->setOut2Selected(false);
        } else {
            b->setIn1Selected(false);
            b->setOut1Selected(false);
            b->setOut2Selected(false);
        }
    }
    //junção
    if(b->getType() == 9) {

        if((mouseX > b->getX()-5) && (mouseX < (b->getX() + 8)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
            b->setIn1Selected(true);
            b->setOut1Selected(false);
            b->setIn2Selected(false);
        } else if((mouseX > b->getX()+22) && (mouseX < (b->getX() + 35)) && (mouseY > b->getY()-5) && (mouseY < (b->getY()+8))) {
            b->setIn1Selected(false);
            b->setOut1Selected(false);
            b->setIn2Selected(true);
        } else if((mouseX > b->getX()+8) && (mouseX < (b->getX() + 21)) && (mouseY > b->getY()+17) && (mouseY < (b->getY()+30))) {
            b->setIn1Selected(false);
            b->setOut1Selected(true);
            b->setIn2Selected(false);
        } else {
            b->setIn1Selected(false);
            b->setOut1Selected(false);
            b->setIn2Selected(false);
        }
    }
}
void Interface :: draw_temporary_line() {
    if(drawing_line == true) {
        int arrow_X1, arrow_Y1, arrow_X2, arrow_Y2;


        int angulo = (atan2((mouseX - temporary_line_X), (mouseY - temporary_line_Y)))*180/M_PI;

        int angulo1 = angulo + 45;
        int angulo2 = angulo - 45;

        //distancia entre os pontos = 10
        //(distancia)2 = (xa - xb)2 + (ya - yb)2
        100 = (mouseX - arrow_X1) + (mouseY - arrow_Y1)
        //https://www.clubedohardware.com.br/forums/topic/780596-resolvido-ajuda-sistema-linear/


        //eq da reta (y - y0) = m(x - x0)

        //cout<<angulo<<endl;
        al_draw_line(temporary_line_X, temporary_line_Y, mouseX, mouseY, black, 2);
        al_draw_filled_triangle(mouseX, mouseY, mouseX-5, mouseY-5, mouseX+5, mouseY-5, black);
    }
}
