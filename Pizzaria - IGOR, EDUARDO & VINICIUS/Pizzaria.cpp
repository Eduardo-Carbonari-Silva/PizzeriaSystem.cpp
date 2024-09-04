#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio2.h>
#include <windows.h>
#include <unistd.h>

// GRUPO: IGOR CARBONARI SILVA, EDUARDO CARBONARI SILVA E VINICIUS DO NASCIMENTO ROCHA

struct TpData{
	int d, m, a;
};

struct TpClientes{
	int cep, cont;
	char nome[30], end[50], cid[30], tel[15], status;
};

struct TpMotoca{
	TpData data;
	char nome[30], end[50], cpf[12], tel[15], status;
	int cont;
};

struct TpPizzas{
	int cod, cont;
	float preco;
	char nome[20], status;
};

struct TpPedidos{
	TpData datapedido;
	int num, codpizza, situacao;
	char cpfmoto[12], tel[15], status;
};

void CriarArq(void){
	FILE *Ptr1 = fopen("Clientes.dat", "ab");
	FILE *Ptr2 = fopen("Motoqueiros.dat", "ab");
	FILE *Ptr3 = fopen("Pedidos.dat", "ab");
	FILE *Ptr4 = fopen("Pizzas.dat", "ab");
	
	fclose(Ptr1);
	fclose(Ptr2);
	fclose(Ptr3);
	fclose(Ptr4);
}

int ValidaCPF(char cpf[12]){
	
	//Se Retornar [1] CPF � v�lido || se retornar [0] CPF � inv�lido
	
	// DIGITO 1
	int soma = 0, j, veri, veri2, div;
	
    for(int i = 0, j = 10; i < 9; j--, i++)
    	soma += (cpf[i] - 48) * j;
    
    div = soma % 11;
    veri = 11 - div;
    
    if(veri >= 10)
    	veri = 0;
    
    // DIGITO 2
    soma = 0;
    
    for(int i = 0, j = 11; i < 10; j--, i++)
    	soma += (cpf[i] - 48) * j;
    
    div = soma % 11;
    veri2 = 11 - div;
    
    if(veri >= 10)
    	veri2 = 0;
    
    // RETURNS
    
	if(veri == (cpf[9] - 48) && veri2 == (cpf[10] - 48)) // CPF VALIDO
		return 1;
	
	else // CPF INVALIDO
		return 0;
}

int BuscaTel(FILE *Ptr, char telaux[15]){ // BUSCA EXAUSTIVA TELEFONES CLIENTES
	
	TpClientes C;
	
	fseek(Ptr, 0, 0);
	
	fread(&C, sizeof(TpClientes), 1, Ptr);
	while(!feof(Ptr) && (stricmp(telaux, C.tel) != 0 || C.status != 'A'))
		fread(&C, sizeof(TpClientes), 1, Ptr);
	
	if(stricmp(telaux, C.tel) == 0)
		return ftell(Ptr) - sizeof(TpClientes);
	
	else
		return -1;
}

int BuscaCPFMotoca(FILE *Ptr, char cpfaux[15]){ // BUSCA EXAUSTIVA CPF MOTOQUEIROS
	
	TpMotoca M;
	
	fseek(Ptr, 0, 0);
		
	fread(&M, sizeof(TpMotoca), 1, Ptr);
	while(!feof(Ptr) && (stricmp(cpfaux, M.cpf) != 0 || M.status != 'A'))
		fread(&M, sizeof(TpMotoca), 1, Ptr);
	
	if(stricmp(cpfaux, M.cpf) == 0)
		return ftell(Ptr) - sizeof(TpMotoca);
	
	else
		return -1;
}

int BuscaSentCPFMotoca(FILE *Ptr, char cpfaux[12]){ // BUSCA SENTINELA CPF MOTOQUEIROS
	
	TpMotoca M;
	TpMotoca sentinela;
	int i = 0;
    
    // Coloca o sentinela no final do arquivo
    fseek(Ptr, 0, 2);
    sentinela.status = 'A';
    strcpy(sentinela.cpf, cpfaux);
    
	int tam = ftell(Ptr) / sizeof(TpMotoca);
    
    fwrite(&sentinela, sizeof(TpMotoca), 1, Ptr);
	
    fseek(Ptr, 0, 0);
	fread(&M, sizeof(TpMotoca), 1, Ptr);
	
    while(stricmp(cpfaux, M.cpf) != 0 || M.status != 'A'){
    	i++;
    	fread(&M, sizeof(TpMotoca), 1, Ptr);
    }
    
    if(i < tam){
		
        // Remove o sentinela
        fseek(Ptr, -sizeof(TpMotoca), 2);
        ftruncate(fileno(Ptr), ftell(Ptr));
		
        return ftell(Ptr) - sizeof(TpMotoca);
    }
    
    // Se chegar aqui, o CPF n�o foi encontrado e remove o sentinela
    fseek(Ptr, -sizeof(TpMotoca), 2);
    ftruncate(fileno(Ptr), ftell(Ptr));
	
    return -1;
}

int BuscaSeqIndexPedido(FILE *Ptr, int numaux){ // BUSCA SEQUENCIAL INDEXADA NUMERO DO PEDIDO
	
	TpPedidos P;
	
	fseek(Ptr, 0, 0);
	
	fread(&P, sizeof(TpPedidos), 1, Ptr);
	while(!feof(Ptr) && (P.num < numaux || P.status != 'A'))
		fread(&P, sizeof(TpPedidos), 1, Ptr);
	
	if (!feof(Ptr) && P.num == numaux)
		return ftell(Ptr) - sizeof(TpPedidos);
	
	else
		return -1;
}

int BuscaBinariaPedido(FILE *Ptr, int numaux){ // BUSCA BINARIA NUMERO DO PEDIDO
	
    TpPedidos P;
    int comeco = 0;
    
    fseek(Ptr, 0, 2);
    int fim = ftell(Ptr) / sizeof(TpPedidos);
    fim -= 1;
    
    int meio = fim / 2;
    
    fseek(Ptr, meio * sizeof(TpPedidos), 0);
    fread(&P, sizeof(TpPedidos), 1, Ptr);
    while(comeco <= fim && (P.num != numaux || P.status != 'A')){
		
        if(numaux > P.num)
            comeco = meio + 1;
        
        else
            fim = meio - 1;
        
        meio = (comeco + fim) / 2;
		
        fseek(Ptr, meio * sizeof(TpPedidos), 0);
        fread(&P, sizeof(TpPedidos), 1, Ptr);
    }
	
    if(comeco <= fim){
    	
    	fseek(Ptr, meio * sizeof(TpPedidos), 0);
    	return ftell(Ptr);
    }
        
    else
        return -1;
}

int BuscaExaustPizza(FILE *Ptr, char nomeaux[20]){ // BUSCA EXAUSTIVA PIZZAS
	
	TpPizzas P;
	
	fseek(Ptr, 0, 0);
	
	fread(&P, sizeof(TpPizzas), 1, Ptr);
	while(!feof(Ptr) && (stricmp(nomeaux, P.nome) != 0 || P.status != 'A'))
		fread(&P, sizeof(TpPizzas), 1, Ptr);
	
	if(stricmp(nomeaux, P.nome) == 0) 
		return ftell(Ptr) - sizeof(TpPizzas);
	
	else
		return -1;
}

int BuscaCod(FILE *Ptr, int codaux){ // BUSCA EXAUSTIVA CODIGOS PIZZAS
	
	TpPizzas P;
	
	fseek(Ptr, 0, 0);
	
	fread(&P, sizeof(TpPizzas), 1, Ptr);
	while(!feof(Ptr) && (codaux != P.cod || P.status != 'A'))
		fread(&P, sizeof(TpPizzas), 1, Ptr);
	
	if(codaux == P.cod) 
		return ftell(Ptr) - sizeof(TpPizzas);
	
	else
		return -1;
}

void InsercaoDireta(void){ // INSERCAO DIRETA PARA CLIENTES DE ACORDO COM QUANTIDADE DE PEDIDOS FEITA
	
	FILE *Ptr = fopen("Clientes.dat", "rb+");

    TpClientes C, aux;

    fseek(Ptr, 0, 2);
    int tam = ftell(Ptr) / sizeof(TpClientes);
        
    for (int i = tam - 2; i >= 0; i--){
        fseek(Ptr, i * sizeof(TpClientes), 0);
        fread(&C, sizeof(TpClientes), 1, Ptr);

        int j = i + 1;
        fseek(Ptr, j * sizeof(TpClientes), 0);
        fread(&aux, sizeof(TpClientes), 1, Ptr);

        while (j < tam && C.cont < aux.cont){
            fseek(Ptr, (j - 1) * sizeof(TpClientes), 0);
            fwrite(&aux, sizeof(TpClientes), 1, Ptr);

            j++;
            if (j < tam) {
                fseek(Ptr, j * sizeof(TpClientes), 0);
                fread(&aux, sizeof(TpClientes), 1, Ptr);
            }
        }
        fseek(Ptr, (j - 1) * sizeof(TpClientes), 0);
        fwrite(&C, sizeof(TpClientes), 1, Ptr);
	}
    
    fclose(Ptr);
}

void SelecaoDireta (void){
	
	FILE *Ptr = fopen("Motoqueiros.dat", "rb");
	
	TpMotoca M, aux;
	int pos_maior = 0;
	
	fseek(Ptr, 0, 2);
	int num_motocas = ftell(Ptr) / sizeof(TpMotoca);
	
	fseek(Ptr, 0, 0);
	fread(&aux, sizeof(TpMotoca), 1, Ptr);
	
	int maior = M.cont;
	
	for(int i = 1; i < num_motocas; i++){
		
		fseek(Ptr, i * sizeof(TpMotoca), 0);
		fread(&M, sizeof(TpMotoca), 1, Ptr);
	
		if(maior < aux.cont){
			maior = aux.cont;
			pos_maior = i;
		}
	}
	
	while(num_motocas > 0){
		
		if(pos_maior < num_motocas - 1){
			
			fseek(Ptr, pos_maior, 0);
			fread(&M, sizeof(TpMotoca), 1, Ptr);
			
			maior = M.cont;
			
			fseek(Ptr, num_motocas - 1, 0);
			fread(&aux, sizeof(TpMotoca), 1, Ptr);
			
			M.cont = aux.cont;
			aux.cont = maior;
		}
		
		num_motocas--;
	}
	
	fclose(Ptr);
}

void BubbleSort(void){ //BubbleSort para RelatorioRankPizza
	
	FILE *Ptr = fopen("Pizzas.dat", "rb+");
	
	if (Ptr == NULL)
        perror("Erro de abertura!\n");
    
    else{
    	
    	TpPizzas P, aux;
		
		fseek(Ptr, 0, 2);
		int tam = ftell(Ptr) / sizeof(TpPizzas);
		fseek(Ptr, 0, 0);
		
	    while(tam > 0){
	    	
	    	for(int j = 0; j < tam - 1; j++){
	        	
	        	// L� o registro j + 1
	        	int tamA = j + 1;
	            fseek(Ptr, tamA * sizeof(TpPizzas), 0);
	            fread(&P, sizeof(TpPizzas), 1, Ptr);
	
	            // L� o registro j
	            int tamB = tamA - 1;
	            fseek(Ptr, tamB * sizeof(TpPizzas), 0);
	            fread(&aux, sizeof(TpPizzas), 1, Ptr);
	            
	            if(P.cont > aux.cont){
					
					// Troca os registros
	                fseek(Ptr, tamA * sizeof(TpPizzas), 0);
	                fwrite(&aux, sizeof(TpPizzas), 1, Ptr);
					
					fseek(Ptr, tamB * sizeof(TpPizzas), 0);
	                fwrite(&P, sizeof(TpPizzas), 1, Ptr);
				}
	        }
	    	
	    	tam--;
	    }
    }
    
    fclose(Ptr);
};

void LimpaTela(void){
	
	for(int i=10; i<28; i++){
		gotoxy(32,i);
		printf("                                                                             ");
	}
}

void CadastrarCliente(void){
	
	TpClientes R;
	
	FILE *Ptr = fopen("Clientes.dat", "rb+");
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> CADASTRAR CLIENTES <----");
	
	gotoxy(32, 11);
	printf("Digite seu nome: ");
	fflush(stdin);
	gets(R.nome);
	
	gotoxy(32, 12);
	printf("Telefone: ");
	fflush(stdin);
	gets(R.tel);
	
	int pos = BuscaTel(Ptr, R.tel);
	
	if(pos != -1){
		gotoxy(32, 13);
		printf("Cliente ja cadastrado!");
	}
	
	else{
		
		gotoxy(32, 13);
		printf("Endereco: ");
		fflush(stdin);
		gets(R.end);
		
		gotoxy(32, 14);
		printf("Cidade: ");
		fflush(stdin);
		gets(R.cid);
		
		gotoxy(32, 15);
		printf("CEP: ");
		scanf("%d", &R.cep);
		
		R.status = 'A';
		
		gotoxy(32, 17);
		printf("Cliente Cadastrado!");
		
		fwrite(&R, sizeof(TpClientes), 1, Ptr);
	}
	
	fclose(Ptr);
	getch();
}

void CadastrarMotoqueiro(void){
	
	TpMotoca M;
	
	FILE *Ptr = fopen("Motoqueiros.dat", "ab+");
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> CADASTRAR MOTOQUEIROS <----");
	
	gotoxy(32, 11);
	printf("Digite seu nome: ");
	fflush(stdin);
	gets(M.nome);
	
	gotoxy(32, 12);
	printf("CPF: ");
	fflush(stdin);
	gets(M.cpf);
	
	int sit = ValidaCPF(M.cpf); // SIT DE SITUA��O
	
	if(sit != 1){
		gotoxy(32, 13);
		printf("CPF invalido!\n");
	}
	
	else{
		
		gotoxy(32, 13);
		printf("Endereco: ");
		fflush(stdin);
		gets(M.end);
		
		gotoxy(32, 14);
		printf("Telefone: ");
		fflush(stdin);
		gets(M.tel);
		
		gotoxy(32, 15);
		printf("Data de Admissao: ");
		scanf("%d %d %d", &M.data.d, &M.data.m, &M.data.a);
		
		M.status = 'A';
		M.cont = 0;
		
		gotoxy(32, 16);
		printf("Motoqueiro Cadastrado!");
		
		fwrite(&M, sizeof(TpMotoca), 1, Ptr);
	}
	
	fclose(Ptr);
	getch();
}

void CadastrarPedido(void){
	
	TpPedidos REG;
	TpPizzas P;
	TpClientes C;
	TpMotoca M;
	
	FILE *Ptr = fopen("Pedidos.dat", "rb+");
	FILE *PtrClientes = fopen("Clientes.dat", "rb+");
	FILE *PtrPizzas = fopen("Pizzas.dat", "rb+");
	FILE *PtrMotoca = fopen("Motoqueiros.dat", "rb+");
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> CADASTRAR PEDIDO <----");
	
	gotoxy(32, 11);	
	printf("Digite o numero do pedido: ");
	scanf("%d", &REG.num);
	
	int busca1 = BuscaBinariaPedido(Ptr, REG.num);
	
	if(busca1 != -1){
		gotoxy(32, 13);
		printf("Numero de pedido ja cadastrado!");
	}
		
	else{
		
		gotoxy(32, 12);
		printf("Seu telefone: ");
		fflush(stdin);
		gets(REG.tel);
		
		int busca2 = BuscaTel(PtrClientes, REG.tel);
		
		if(busca2 != -1){
			
			gotoxy(32, 13);
			printf("Codigo da pizza: ");
			scanf("%d", &REG.codpizza);
			
			int busca3 = BuscaCod(PtrPizzas, REG.codpizza);
			
			if(busca3 != -1){
				
				gotoxy(32, 14);
				printf("CPF do motoqueiro: ");
				fflush(stdin);
				gets(REG.cpfmoto);
				
				int busca4 = BuscaSentCPFMotoca(PtrMotoca, REG.cpfmoto);
				
				if(busca4 != -1){
					
					gotoxy(32, 15);
					printf("Digite a data do pedido: ");
					scanf("%d %d %d", &REG.datapedido.d, &REG.datapedido.m, &REG.datapedido.a);
					
					REG.status = 'A';
					REG.situacao = 1;
					
					fseek(Ptr, 0, 2);
					fwrite(&REG, sizeof(TpPedidos), 1, Ptr);
					
					gotoxy(32, 16);
					printf("Pedido Cadastrado!");
					
					//ORDENAR PIZZAS PELA QUANTIDADE PEDIDA
					fseek(PtrPizzas, 0, 2);
				    int num_pizzas = ftell(PtrPizzas) / sizeof(TpPizzas);
					
				    for(int i = 0; i < num_pizzas; i++){
				    	
				    	fseek(PtrPizzas, i * sizeof(TpPizzas), 0);
			            fread(&P, sizeof(TpPizzas), 1, PtrPizzas);
			            
			            P.cont = 0;
			            
			            fseek(Ptr, 0, 0);
				    	while(fread(&REG, sizeof(TpPedidos), 1, Ptr) != 0)
				    		if(REG.codpizza == P.cod)
				    			P.cont++;
				    	
				    	fseek(PtrPizzas, i * sizeof(TpPizzas), 0);
				    	fwrite(&P, sizeof(TpPizzas), 1, PtrPizzas);
				    }
				    
				    fclose(PtrPizzas);
				    
				    BubbleSort();
				    
				    PtrPizzas = fopen("Pizzas.dat", "rb+");
				    //ORDENAR PIZZAS PELA QUANTIDADE PEDIDA
				    
				    //ORDENAR CLIENTES PELA QUANTIDADE DE PEDIDOS
				    fseek(PtrClientes, 0, 2);
				    int num_clientes = ftell(PtrClientes) / sizeof(TpClientes);
				    
				    for(int i = 0; i < num_clientes; i++){
				    	
				    	fseek(PtrClientes, i * sizeof(TpClientes), 0);
			            fread(&C, sizeof(TpClientes), 1, PtrClientes);
			            
			            C.cont = 0;
			            
			            fseek(Ptr, 0, 0);
			            while(fread(&REG, sizeof(TpPedidos), 1, Ptr) != 0)
			            	if(stricmp(REG.tel, C.tel) == 0)
			            		C.cont++;
			            
			            fseek(PtrClientes, i * sizeof(TpClientes), 0);
			            fwrite(&C, sizeof(TpClientes), 1, PtrClientes);
				    }
				    
				    fclose(PtrClientes);
				    
				    InsercaoDireta();
				    
				    PtrClientes = fopen("Clientes.dat", "rb+");
				    //ORDENAR CLIENTES PELA QUANTIDADE DE PEDIDOS
				    
				    //ORDENAR MOTOCAS PELA QUANTIDADE DE ENTREGAS
				    fseek(PtrMotoca, 0, 2);
				    int num_motocas = ftell(PtrMotoca) / sizeof(TpMotoca);
				    
				    for(int i = 0; i < num_motocas; i++){
				    	
				    	fseek(PtrMotoca, i * sizeof(TpMotoca), 0);
			            fread(&M, sizeof(TpMotoca), 1, PtrMotoca);
			            
			            M.cont = 0;
			            
			            fseek(Ptr, 0, 0);
			            while(fread(&REG, sizeof(TpPedidos), 1, Ptr) != 0)
			            	if(stricmp(REG.cpfmoto, M.cpf) == 0)
			            		M.cont++;
			            
			            fseek(PtrMotoca, i * sizeof(TpMotoca), 0);
			            fwrite(&M, sizeof(TpMotoca), 1, PtrMotoca);
				    }
				    
				    fclose(PtrMotoca);
				    
				    SelecaoDireta();
				    
				    PtrMotoca = fopen("Motoqueiros.dat", "rb+");
				    //ORDENAR MOTOCAS PELA QUANTIDADE DE ENTREGAS
					
					gotoxy(32, 17);
					printf("Situacao: 1 - Em preparo!");
				}
				
				else{
					gotoxy(32, 15);
					printf("CPF nao existe!");
				}
			}
			
			else{
				gotoxy(32, 14);
				printf("Pizza nao existe!");
			}
		}
		
		else{
			gotoxy(32, 13);
			printf("Telefone Invalido!");
		}
	}
	
	fclose(Ptr);
	fclose(PtrClientes);
	fclose(PtrPizzas);
	fclose(PtrMotoca);
	getch();
}

void CadastrarPizza(void){
	
	TpPizzas R;
	
	FILE *Ptr = fopen("Pizzas.dat", "rb+");
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> CADASTRAR PIZZAS <----");
	
	gotoxy(32, 11);
	printf("Digite o nome da pizza: ");
	fflush(stdin);
	gets(R.nome);
	
	int pos = BuscaExaustPizza(Ptr, R.nome);
	
	if(pos != -1){
		gotoxy(32, 12);
		printf("Pizza ja cadastrada!\n");
	}
	
	else{
		
		gotoxy(32, 12);
		printf("Codigo: ");
		scanf("%d", &R.cod);
		
		gotoxy(32, 13);
		printf("Valor: ");
		scanf("%f", &R.preco);
		
		R.status = 'A';
		
		gotoxy(32, 14);
		printf("Pizza Cadastrada!");
		
		fwrite(&R, sizeof(TpPizzas), 1, Ptr);
	}
	
	fclose(Ptr);
	getch();
}

char MenuCadastros(void){
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> CADASTROS <----");
	gotoxy(32, 11);
	printf("[A] Cadastrar Cliente");
	gotoxy(32, 12);
	printf("[B] Cadastrar Motoqueiros");
	gotoxy(32, 13);
	printf("[C] Cadastrar Pedidos");
	gotoxy(32, 14);
	printf("[D] Cadastrar Pizzas");
	gotoxy(32, 15);
	printf("[ESC] Voltar");
	gotoxy(32, 17);
	printf("Opcao: ");
	
	return toupper(getch());
}

void Cadastros(void){
	
	char op;
	
	do{
		
		op = MenuCadastros();
		
		switch(op){
			
			case 'A': CadastrarCliente();
					  break;
			
			case 'B': CadastrarMotoqueiro();
					  break;
			
			case 'C': CadastrarPedido();
					  break;
			
			case 'D': CadastrarPizza();
		}
		
	}while(op != 27);
}

int ValidaData (int d, int m, int a){ //1 --> true | 0 --> false

    if (a < 1)
        return 0;

    if (m < 1 || m > 12)
        return 0;

    if (d < 1 || d > 31)
        return 0;

    // Verificar os dias de cada m�s
    
    if (m == 2) // Fevereiro
        if (d > 28)
            return 0;
	
	else if (m == 4 || m == 6 || m == 9 || m == 11) // Abril, Junho, Setembro, Novembro
        if (d > 30)
            return 0;
	
	else // Janeiro, Mar�o, Maio, Julho, Agosto, Outubro, Dezembro
        if (d > 31)
            return 0;

    return 1;
}

int ValidaIntervaloData (int dia1, int mes1, int ano1, int dia2, int mes2, int ano2){ //1 --> true | 0 --> false

    if (ano1 < ano2)
		return 1;
    if (ano1 > ano2)
		return 0;
    if (mes1 < mes2)
		return 1;
    if (mes1 > mes2)
		return 0;
    if (dia1 <= dia2)
		return 1;
	
    return 0;
}

void RelatorioMotocaPizza(void){
	
	FILE * PtrPedido = fopen("Pedidos.dat", "rb");
    FILE * PtrMotoca = fopen("Motoqueiros.dat", "rb");
	
    TpMotoca M;
    TpPedidos P;
	
    int dia1, dia2, mes1, mes2, ano1, ano2;
    int count, y = 13;
	
    LimpaTela();
	
    if(PtrPedido == NULL || PtrMotoca == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura!");
    }
	
    else{
		
		gotoxy(32, 10);
        printf("----> DIGITE AS DATAS <----");
		
		gotoxy(32, 11);
        printf("Digite a data inicial (dd mm aaaa): ");
        scanf("%d %d %d", &dia1, &mes1, &ano1);
        
        int busca1 = ValidaData(dia1, mes1, ano1);
        
        if(busca1 != 0){
        	
        	gotoxy(32, 12);
        	printf("Digite a data final (dd mm aaaa): ");
	        scanf("%d %d %d", &dia2, &mes2, &ano2);
	        
	        int busca2 = ValidaData(dia2, mes2, ano2);
	        
	        if(busca2 != 0){
	        	
	        	int busca3 = ValidaIntervaloData(dia1, mes1, ano1, dia2, mes2, ano2);
	        	
	        	if(busca3 != 0){
	        		
	        		LimpaTela();
	    			
	    			gotoxy(32, 10);
			        printf("----> MOTOQUEIROS E SUAS ENTREGAS <----");
			        gotoxy(32, 11);
			        printf("%d/%d/%d --------------------- %d/%d/%d\n", dia1, mes1, ano1, dia2, mes2, ano2);
			    
			        while(fread(&M, sizeof(TpMotoca), 1, PtrMotoca) != 0){
						
			            count = 0;
			            fseek(PtrPedido, 0, 0);
			            
			            while(fread(&P, sizeof(TpPedidos), 1, PtrPedido) != 0)
							if(P.situacao == 3)
								if(stricmp(M.cpf, P.cpfmoto) == 0)
				                	if(P.datapedido.d >= dia1 && P.datapedido.d <= dia2)
				                		if(P.datapedido.m >= mes1 && P.datapedido.m <= mes2)
				                			if(P.datapedido.a >= ano1 && P.datapedido.a <= ano2)
					                    		count++;
			            
			    		gotoxy(32, y++);
			            printf("Motoqueiro: %s", M.nome);
			            gotoxy(32, y++);
			            printf("Pizzas Entregues: %d\n", count);
			            y++;
			        }
	        	}
	        	
	        	else{
	        		gotoxy(32, 13);
	        		printf("Intervalo de data invalida!\n");
	        	}
	        }
	        
	        else{
	        	gotoxy(32, 13);
	        	printf("Data Invalida!\n");
	        }
        }
        
        else{
        	gotoxy(32, 12);
        	printf("Data Invalida!\n");
        }
    
        fclose(PtrPedido);
        fclose(PtrMotoca);
    }
    
    getch();
}

void RelatorioInicialCliente(char inicial){
	
	FILE * Ptr = fopen("Clientes.dat", "rb");

    TpClientes C;
    int encontrou = 0, y = 15;

    if(Ptr == NULL){
    	gotoxy(32, 12);
    	printf("Erro de abertura!\n");
    }

    else{
    	
    	gotoxy(32, 13);
        printf("----> INICIAL: %c <----", inicial);
        
        fseek(Ptr, 0, 0);
        
        while(fread(&C, sizeof(TpClientes), 1, Ptr) != 0)
        	if(C.status == 'A')
	            if(C.nome[0] == inicial || C.nome[0] == toupper(inicial)){
	            	encontrou = 1;
	            	gotoxy(32, y++);
	                printf("Cliente: %s\n", C.nome);
	                gotoxy(32, y++);
	                printf("Telefone: %s\n", C.tel);
	                gotoxy(32, y++);
	                printf("Endereco: %s\n", C.end);
	                gotoxy(32, y++);
	                printf("Cidade: %s\n", C.cid);
	                gotoxy(32, y++);
	                printf("CEP: %d\n", C.cep);
	            }
    	
    	if(encontrou == 0){
    		gotoxy(32, 15);
    		printf("Nenhum cliente encontrado com a inicial '%c'\n", inicial);
    	}
    	
        fclose(Ptr);
    }
    
    getch();
}

void RelatorioRankPizza(void){
	
    TpPizzas Pizza;
    int num_pizzas;
    
    LimpaTela();

    FILE *PtrPedido = fopen("Pedidos.dat", "rb");
    FILE *PtrPizza = fopen("Pizzas.dat", "rb+");

    if (PtrPedido == NULL || PtrPizza == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura!\n");
    }
    
	else{
		
		gotoxy(32, 10);
		printf("----> RANK DE PIZZAS <----");
		
		fseek(PtrPizza, 0, 2);
		num_pizzas = ftell(PtrPizza) / sizeof(TpPizzas);
		
		int y = 12;
	    
	    for(int i = 0; i < num_pizzas; i++){
	    	
	    	fseek(PtrPizza, i * sizeof(TpPizzas), 0);
	    	fread(&Pizza, sizeof(TpPizzas), 1, PtrPizza);
	    	
	    	gotoxy(32, y++);
	    	printf("%dx %s\n", Pizza.cont, Pizza.nome);
	    }
	}
	
	fclose(PtrPizza);

    getch();
}

void RelatorioClientes(void){
	
    FILE *PtrCliente = fopen("Clientes.dat", "rb");
    FILE *PtrPedido = fopen("Pedidos.dat", "rb");
    FILE *PtrPizza = fopen("Pizzas.dat", "rb");

    TpClientes C;
    TpPedidos P;
    TpPizzas Pizza;
    float total;
    int y = 12;

    LimpaTela();

    if(PtrCliente == NULL || PtrPedido == NULL || PtrPizza == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura!\n");
    }

    else{
		
        fseek(PtrCliente, 0, 0);
    	
        while(fread(&C, sizeof(TpClientes), 1, PtrCliente) != 0){
        		
        	if(C.status == 'A'){
        		
        		gotoxy(32, 10);
        		printf("----> CLIENTES E SEUS PEDIDOS <----");
        			
    			gotoxy(32, y++);
        		printf("----> DADOS DO CLIENTE <----");
        		gotoxy(32, y++);
	            printf("Cliente: %s", C.nome);
	            gotoxy(32, y++);
	            printf("Telefone: %s", C.tel);
	            gotoxy(32, y++);
	            printf("Endereco: %s", C.end);
	            gotoxy(32, y++);
	            printf("Cidade: %s", C.cid);
	            gotoxy(32, y++);
	            printf("CEP: %d", C.cep);
	            y++;
	            gotoxy(32, y++);
	            printf("\t\tPEDIDOS:");
	            
	            int encontrou = 0;
	            total = 0;
	            
	            fseek(PtrPedido, 0, 0);
	    		
	            while(fread(&P, sizeof(TpPedidos), 1, PtrPedido) != 0)
		        	if(stricmp(P.tel, C.tel) == 0){
		        		
		        		encontrou = 1;
		        		fseek(PtrPizza, 0, 0);
		        		
		        		while(fread(&Pizza, sizeof(TpPizzas), 1, PtrPizza) != 0)
		                	if(Pizza.cod == P.codpizza){
		                		
		                		gotoxy(32, y++);
		                		printf("Pizzas Pedidas: %s", Pizza.nome);
		                		gotoxy(32, y++);
		                		printf("Preco: R$ %.2f", Pizza.preco);
		                		total += Pizza.preco;
		                	}
		        	}
		    	
		        if (encontrou == 0){
		        	y++;
		        	gotoxy(32, y++);
		            printf("Cliente sem pedidos.");
		        }
		        
	            y++;
	            gotoxy(77, 19);
	            printf("Total Gasto: R$ %.2f", total);
	            gotoxy(75, 20);
	            printf("--------------------------");
	            
	            for(int j = 5; j > 0; j--){
	            	
	            	gotoxy(79, 27);
	            	printf("Proxima pagina em %d segundos!", j);
	            	Sleep(1000);
	            }
	            
	            LimpaTela();
				
				y = 12;
        	}
        }
    }
    
    fclose(PtrCliente);
    fclose(PtrPedido);
    fclose(PtrPizza);
    
    getch();
}

void RelatorioPizzasEmPreparo(void){
	
	TpPedidos P;
	TpPizzas Pizza;
	int y = 12;
	
	FILE *Ptr = fopen("Pedidos.dat", "rb");
	FILE *PtrPizza = fopen("Pizzas.dat", "rb");
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> PIZZAS EM PREPARO <----\n");
	
	fseek(Ptr, 0, 0);
	
	while(fread(&P, sizeof(TpPedidos), 1, Ptr) != 0){
		
		if (P.situacao == 1) {
			
            fseek(PtrPizza, 0, 0);
            
            while (fread(&Pizza, sizeof(TpPizzas), 1, PtrPizza) != 0) {
                if (P.codpizza == Pizza.cod){
                	gotoxy(32, y++);
                    printf("Pizza em preparo: %s", Pizza.nome);
                    fseek(PtrPizza, 0 , 2);
            	}
        	}
		}
	}
	
	fclose(Ptr);
	fclose(PtrPizza);
	getch();
}

void RelatorioPizzasACaminho(void){
	
	TpPizzas Pizza;
	TpPedidos P;
	int y = 12;
	
	FILE *Ptr = fopen("Pedidos.dat", "rb");
	FILE *PtrPizza = fopen ("Pizzas.dat", "rb");
		
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> PIZZAS A CAMINHO <----\n");
	
	fseek(Ptr, 0, 0);
	
	while(fread(&P, sizeof(TpPedidos), 1, Ptr) != 0){
		
		if (P.situacao == 2) {
			
            fseek(PtrPizza, 0, 0);
            
            while (fread(&Pizza, sizeof(TpPizzas), 1, PtrPizza) != 0) {
                if (P.codpizza == Pizza.cod){
                	gotoxy(32, y++);
                    printf("Pizza a caminho: %s", Pizza.nome);
                    fseek(PtrPizza, 0 , 2);
            	}
        	}
		}
	}
	
	fclose(PtrPizza);
	fclose(Ptr);
	getch();
}

char MenuRelatorios(void){
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> RELATORIOS <----");
	gotoxy(32, 11);
	printf("[A] Pizzas Entregues e seus Respectivos Entregadores em um Determinado Dia");
	gotoxy(32, 12);
	printf("[B] Por Inicial de Cliente");
	gotoxy(32, 13);
	printf("[C] Ranking de Pizzas");
	gotoxy(32, 14);
	printf("[D] Todos os Clientes");
	gotoxy(32, 15);
	printf("[E] Pizzas em Preparo");
	gotoxy(32, 16);
	printf("[F] Pizzas a Caminho");
	gotoxy(32, 17);
	printf("[ESC] Voltar");
	gotoxy(32, 19);
	printf("Opcao: ");
	
	return toupper(getch());	
}

void Relatorios(void){
	
	char op, inicial;
	
	do{
		
		op = MenuRelatorios();
		
		switch(op){
			
			case 'A': RelatorioMotocaPizza();
					  break;
			
			case 'B': LimpaTela();
					  gotoxy(32, 10);
					  printf("----> RELATORIO POR INICIAL <----");
					  gotoxy(32, 11);
					  printf("Digite a inicial: ");
        			  scanf(" %c", &inicial);
					  RelatorioInicialCliente(inicial);
					  break;
			
			case 'C': RelatorioRankPizza();
					  break;
			
			case 'D': RelatorioClientes();
					  break;
			
			case 'E': RelatorioPizzasEmPreparo();
					  break;
					  
			case 'F': RelatorioPizzasACaminho();
		}
		
	}while(op != 27);
}

void ConsultarPizzaMaisPedida(void){
	 
	 FILE * Ptr = fopen("Pizzas.dat", "rb");
	 
	 LimpaTela();
	 
	 TpPizzas P;
	 
	 gotoxy(32, 10);
	 printf("----> PIZZA MAIS PEDIDA <----");
	 
	 fseek(Ptr, 0, 0);
	 fread(&P, sizeof(TpPizzas), 1, Ptr);
	 
	 gotoxy(32, 12);
	 printf("Pizza mais pedida: %s", P.nome);
	 gotoxy(32, 13);
	 printf("Quantidade Pedida: %d", P.cont);
	 
	 fclose(Ptr);
	 
	 getch();
}

void ConsultarPizzaMenosPedida(void){
	
	FILE * Ptr = fopen("Pizzas.dat", "rb");
	
	LimpaTela();
	
	TpPizzas P;
	
	gotoxy(32, 10);
	printf("----> PIZZA MENOS PEDIDA <----");
	
	fseek(Ptr, 0, 2);
	int tam = ftell(Ptr) / sizeof(TpPizzas);
	
	fseek(Ptr, (tam - 1) * sizeof(TpPizzas), 0);
	fread(&P, sizeof(TpPizzas), 1, Ptr);
	
	gotoxy(32, 12);
	printf("Pizza menos pedida: %s", P.nome);
	gotoxy(32, 13);
	printf("Quantidade Pedida: %d", P.cont);
	
	fclose(Ptr);
	
	getch();
}

void ConsultarClienteMaisConsome(void){

    FILE *Ptr = fopen("Clientes.dat", "rb");

    TpClientes C;
    
    LimpaTela();
    
    gotoxy(32, 10);
    printf("----> CLIENTE QUE MAIS CONSOME <----");
	
	fread(&C, sizeof(TpClientes), 1, Ptr);
	
	while(!feof(Ptr) && C.status != 'A')
    	fread(&C, sizeof(TpClientes), 1, Ptr);
	
	if(!feof(Ptr)){
		
		gotoxy(32, 12);
	    printf("Cliente: %s", C.nome);
	    gotoxy(32, 13);
	    printf("Telefone: %s", C.tel);
	    gotoxy(32, 14);
	    printf("Endereco: %s", C.end);
	    gotoxy(32, 15);
	    printf("Cidade: %s", C.cid);
	    gotoxy(32, 16);
	    printf("CEP: %d", C.cep);
	}
	
	else{
		gotoxy(32, 12);
		printf("Nao ha clientes!");
	}
	
    fclose(Ptr);
	
    getch();
}

void ConsultarClienteMaisConsomeUmaPizza(void){
	
	TpPedidos pedidos;
	TpClientes clientes;
	TpPizzas pizzas;
	
	LimpaTela();
	
	int x, valor = 0, valorreal = 0, ptr = 0, ptrreal;
	
	FILE *ptrpedidos = fopen("Pedidos.dat","rb+");
	FILE *ptrclientes = fopen("Clientes.dat","rb+");
	FILE *ptrpizzas = fopen("Pizzas.dat","rb+");
	
	if(ptrpedidos == NULL || ptrclientes == NULL || ptrpizzas == NULL)
    	printf("Erro de Abertura de Arquivo\n");
    
	else{
		
		gotoxy(32, 10);
    	printf("----> CLIENTE QUE MAIS CONSOME UMA DETERMINADA PIZZA <----");
		
		gotoxy(32, 12);
		printf("Digite o codigo da pizza: ");
		scanf("%d", &pizzas.cod);
		
		x = BuscaCod(ptrpizzas, pizzas.cod);
		
		if(x == -1){
			gotoxy(32, 14);
			printf("Pizza nao existe\n");
		}
		
		else{
			
			fseek(ptrclientes,0,0);
			fread(&clientes,sizeof(clientes),1,ptrclientes);
			
			while(!feof(ptrclientes)){
				
				valor = 0;
				fseek(ptrpedidos,0,0);
				fread(&pedidos,sizeof(pedidos),1,ptrpedidos);
				
				while(!feof(ptrpedidos)){
					
					if(pizzas.cod == pedidos.codpizza && stricmp(clientes.tel,pedidos.tel) == 0){
						valor++;
					}
				
					fread(&pedidos,sizeof(pedidos),1,ptrpedidos);
				}
				
				if(valorreal < valor && clientes.status == 'A'){
					valorreal = valor;
					ptrreal = ptr;
				}
				
				ptr++;
	
				fread(&clientes,sizeof(clientes),1,ptrclientes);
			}
			
			fseek(ptrclientes,ptrreal * sizeof(clientes),0);
			fread(&clientes,sizeof(clientes),1,ptrclientes);
			
			gotoxy(32, 14);
			printf("Telefone: %s\n", clientes.tel);
			gotoxy(32, 15);
			printf("Nome: %s\n", clientes.nome);
			gotoxy(32, 16);
			printf("CEP: %d\n", clientes.cep);
			gotoxy(32, 17);
			printf("Endereco: %s\n", clientes.end);
			gotoxy(32, 18);
			printf("Cidade: %s\n", clientes.cid);
			gotoxy(32, 19);
			printf("Quantidade pedidas dessa pizza: %d\n", valorreal);
		}
	}
	
	fclose(ptrpedidos);
	fclose(ptrpizzas);
	fclose(ptrclientes);
	
	getch();
}

void ConsultarMotoMenosExp(void){
	
	TpMotoca motoqueiros;
    int d, m, a, certo = 0, i = 0;
    
    FILE *ptrmotoqueiros = fopen("Motoqueiros.dat", "rb+");
    
    LimpaTela();
	
    if(ptrmotoqueiros == NULL){
    	gotoxy(32, 10);
        printf("ERRO NA ABERTURA");
    }
    
    else{
    	
    	gotoxy(32, 10);
    	printf("----> MOTOQUEIRO MENOS EXPERIENTE <----");
    	
        fseek(ptrmotoqueiros, 0, 0);
        fread(&motoqueiros, sizeof(motoqueiros), 1, ptrmotoqueiros);
        
        d = motoqueiros.data.d;
        m = motoqueiros.data.m;
        a = motoqueiros.data.a;
        
        fread(&motoqueiros, sizeof(motoqueiros), 1, ptrmotoqueiros);
        while(!feof(ptrmotoqueiros)){
        	
        	i++;
        	
        	if(motoqueiros.status != 'I'){
			
	            if(a <= motoqueiros.data.a)
	                if(m <= motoqueiros.data.m)
	                    if(d <= motoqueiros.data.d){
	                        d = motoqueiros.data.d;
	                        m = motoqueiros.data.m;
	                        a = motoqueiros.data.a;
	                        certo = i;
	                    }
        	}
            
            fread(&motoqueiros,sizeof(motoqueiros),1,ptrmotoqueiros);
        }
		
        fseek(ptrmotoqueiros, certo * sizeof(motoqueiros), 0);
        fread(&motoqueiros, sizeof(motoqueiros), 1, ptrmotoqueiros);
        
        gotoxy(32, 12);
        printf("CPF: %s", motoqueiros.cpf);
        gotoxy(32, 13);
        printf("Nome: %s", motoqueiros.nome);
        gotoxy(32, 14);
        printf("Telefone: %s", motoqueiros.tel);
        gotoxy(32, 15);
        printf("Endereco: %s", motoqueiros.end);
        gotoxy(32, 16);
        printf("Data de admissao: %d/%d/%d", motoqueiros.data.d, motoqueiros.data.m, motoqueiros.data.a);
    }
    
    fclose(ptrmotoqueiros);
	
    getch();
}

void ConsultarMotocaQueMaisEntregouEmUmDia(void){
	
	TpMotoca motoqueiros;
	TpPedidos pedidos;
	
	LimpaTela();
	
	int ptr = 0, ptrreal, valor, valorreal = 0;
	int d, m, a;
	
	FILE *ptrpedidos = fopen("Pedidos.dat","rb+");
	FILE *ptrmotoqueiros = fopen("Motoqueiros.dat","rb+");
	
	gotoxy(32, 10);
    printf("----> MOTOQUEIRO QUE MAIS ENTREGOU EM X DIA <----");
	
	if(ptrpedidos == NULL || ptrmotoqueiros == NULL){
		gotoxy(32, 12);
		printf("Erro de Abertura de Arquivo\n");
	}
    
	else{
		
		gotoxy(32, 12);
		printf("Digite o dia: ");
		scanf("%d %d %d", &d,&m,&a);
		
		fseek(ptrmotoqueiros,0,0);
		fread(&motoqueiros, sizeof(motoqueiros), 1, ptrmotoqueiros);
			
		while(!feof(ptrmotoqueiros)){
				
			valor = 0;
			fseek(ptrpedidos,0,0);
			fread(&pedidos, sizeof(pedidos), 1, ptrpedidos);
				
			while(!feof(ptrpedidos)){
					
				if(d == pedidos.datapedido.d && stricmp(motoqueiros.cpf,pedidos.cpfmoto) == 0)
					if(m == pedidos.datapedido.m && a == pedidos.datapedido.a)
						valor ++;
					
				fread(&pedidos,sizeof(pedidos),1,ptrpedidos);
			}
				
			if(valorreal < valor && motoqueiros.status == 'A'){
				valorreal = valor;
				ptrreal = ptr;
			}
				
			ptr++;

			fread(&motoqueiros, sizeof(motoqueiros), 1, ptrmotoqueiros);
		}
			
		fseek(ptrmotoqueiros, ptrreal * sizeof(motoqueiros), 0);
		fread(&motoqueiros, sizeof(motoqueiros), 1, ptrmotoqueiros);
			
		if(valorreal == 0){
			gotoxy(32, 14);
			printf("Nenhuma entrega foi realizada nessa data!");
		}
			
		else{
				
			gotoxy(32, 14);
			printf("CPF: %s\n", motoqueiros.cpf);
			gotoxy(32, 15);
			printf("Nome: %s\n", motoqueiros.nome);
			gotoxy(32, 16);
			printf("Telefone: %s\n", motoqueiros.tel);
			gotoxy(32, 17);
			printf("Endereco: %s\n", motoqueiros.end);
			gotoxy(32, 18);
			printf("Data de admissao: %d/%d/%d\n", motoqueiros.data.d,motoqueiros.data.m,motoqueiros.data.a);
			gotoxy(32, 19);
			printf("Quantidade entregue nesse dia: %d\n", valorreal);
		}
	}
	
	fclose(ptrpedidos);
	fclose(ptrmotoqueiros);
	
	getch();
}

char MenuConsultas(void){
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> CONSULTAS <----");
	gotoxy(32, 11);
	printf("[A] Pizza Mais Pedida");
	gotoxy(32, 12);
	printf("[B] Pizza Menos Pedida");
	gotoxy(32, 13);
	printf("[C] Cliente que Mais Consome");
	gotoxy(32, 14);
	printf("[D] Cliente que Mais Consome uma Determinada Pizza");
	gotoxy(32, 15);
	printf("[E] Motoqueiro Menos Experiente");
	gotoxy(32, 16);
	printf("[F] Motoqueiro que Mais Entregou em um Determinado Dia");
	gotoxy(32, 17);
	printf("[ESC] Voltar");
	gotoxy(32, 19);
	printf("Opcao: ");
	
	return toupper(getch());
}

void Consultas(void){
	
	char op;
	
	do{
		
		op = MenuConsultas();
		
		switch(op){
			
			case 'A': ConsultarPizzaMaisPedida();
					  break;
			
			case 'B': ConsultarPizzaMenosPedida();
					  break;
			
			case 'C': ConsultarClienteMaisConsome();
					  break;
			
			case 'D': ConsultarClienteMaisConsomeUmaPizza();
					  break;
			
			case 'E': ConsultarMotoMenosExp();
					  break;
					  
			case 'F': ConsultarMotocaQueMaisEntregouEmUmDia();
		}
		
	}while(op != 27);
}

void AlterarCliente(void){

    LimpaTela();
    
    TpClientes C;
    char op;

    FILE *Ptr = fopen("Clientes.dat", "rb+");

    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro ao abrir o arquivo");
    }
	
    else{
		
		gotoxy(32, 10);
		printf("----> ALTERAR CLIENTE <----");
		
		gotoxy(32, 12);
        printf("Digite o telefone para alteracao de dados: ");
        fflush(stdin);
        gets(C.tel);
		
        int busca = BuscaTel(Ptr, C.tel);
		
        if(busca != -1){
        	
        	fseek(Ptr, busca, 0);
        	fread(&C, sizeof(TpClientes), 1, Ptr);
        	
        	gotoxy(32, 14);
    		printf("Nome: %s", C.nome);
    		gotoxy(32, 15);
    		printf("Telefone: %s", C.tel);
    		gotoxy(32, 16);
    		printf("Endereco: %s", C.end);
    		gotoxy(32, 17);
    		printf("Cidade: %s", C.cid);
    		gotoxy(32, 18);
    		printf("CEP: %d", C.cep);
    		
    		gotoxy(32, 20);
    		printf("Continuar exclusao? (S/N): ");
    		op = toupper(getche());
    		
    		if(op == 'S'){
    			
    			LimpaTela();
    			
    			fseek(Ptr, busca, 0);
    			
    			gotoxy(32, 10);
				printf("----> ALTERAR CLIENTE <----");
			
				gotoxy(32, 12);
	            printf("O que deseja alterar?");
	            gotoxy(32, 13);
	            printf("[A] Nome");
	            gotoxy(32, 14);
	            printf("[B] Telefone");
	            gotoxy(32, 15);
	            printf("[C] Endereco");
	            gotoxy(32, 16);
	            printf("[D] Cidade");
	            gotoxy(32, 17);
	            printf("[E] CEP");
	            gotoxy(32, 19);
	            printf("Opcao: ");
	            fflush(stdin);
	            op = toupper(getche());
	            
	            LimpaTela();
	            gotoxy(32, 10);
				printf("----> ALTERAR CLIENTE <----");
				
	            if(op == 'A' || op == 'a'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Nome: ");
	                fflush(stdin);
	                gets(C.nome);
	            }
				
	            else if(op == 'B' || op == 'b'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Telefone: ");
	                fflush(stdin);
	                gets(C.tel);
	            }
				
	            else if(op == 'C' || op == 'c'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Endereco: ");
	                fflush(stdin);
	                gets(C.end);
	            }
				
	            else if(op == 'D' || op == 'd'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Cidade: ");
	                fflush(stdin);
	                gets(C.cid);
	            }
				
	            else if(op == 'E' || op == 'e'){
					
					gotoxy(32, 12);
	                printf("Digite o novo CEP: ");
	                scanf("%d", &C.cep);
	            }
	            
	            fseek(Ptr, sizeof(TpClientes) * busca, 0);
	            fwrite(&C, sizeof(TpClientes), 1, Ptr);
				
				gotoxy(32, 14);
	            printf("Registro Alterado!\n");
	    	}	
        }
		
        else{
        	gotoxy(32, 14);
        	printf("Telefone invalido!\n");
        }
        
        fclose(Ptr);
    }
    
    getch();
}

void AlterarMotoca(void){

    LimpaTela();
    
    TpMotoca M;
    char op;

    FILE *Ptr = fopen("Motoqueiros.dat", "rb+");

    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro ao abrir o arquivo");
    }

    else{
    	
    	gotoxy(32, 10);
    	printf("----> ALTERAR MOTOQUEIRO <----");
		
		gotoxy(32, 12);
        printf("Digite o CPF para alteracao de dados: ");
        fflush(stdin);
        gets(M.cpf);

        int busca = BuscaSentCPFMotoca(Ptr, M.cpf);

        if(busca != -1){
        	
        	fseek(Ptr, busca, 0);
        	fread(&M, sizeof(TpMotoca), 1, Ptr);
        	
        	gotoxy(32, 14);
    		printf("Nome: %s", M.nome);
    		gotoxy(32, 15);
    		printf("Telefone: %s", M.tel);
    		gotoxy(32, 16);
    		printf("Endereco: %s", M.end);
    		gotoxy(32, 17);
    		printf("CPF: %s", M.cpf);
    		gotoxy(32, 18);
    		printf("Data de admissao: %d/%d/%d", M.data.d, M.data.m, M.data.a);
    		
    		gotoxy(32, 20);
    		printf("Continuar exclusao? (S/N): ");
    		op = toupper(getche());
    		
    		if(op == 'S'){
    			
    			LimpaTela();
    			
    			gotoxy(32, 10);
    			printf("----> ALTERAR MOTOQUEIRO <----");
    			
	    		fseek(Ptr, busca, 0);
				
				gotoxy(32, 12);
	            printf("O que deseja alterar?");
	            gotoxy(32, 13);
	            printf("[A] Nome");
	            gotoxy(32, 14);
	            printf("[B] CPF");
	            gotoxy(32, 15);
	            printf("[C] Endereco");
	            gotoxy(32, 16);
	            printf("[D] Telefone");
	            gotoxy(32, 17);
	            printf("[E] Data de Admissao");
	            gotoxy(32, 19);
	            printf("Opcao: ");
	            fflush(stdin);
	            op = toupper(getche());
	            
	            LimpaTela();
	            gotoxy(32, 10);
	    		printf("----> ALTERAR MOTOQUEIRO <----");
	
	            if(op == 'A' || op == 'a'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Nome: ");
	                fflush(stdin);
	                gets(M.nome);
	            }
	
	            else if(op == 'B' || op == 'b'){
					
					gotoxy(32, 12);
	                printf("Digite o novo CPF: ");
	                fflush(stdin);
	                gets(M.cpf);
	            }
	
	            else if(op == 'C' || op == 'c'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Endereco: ");
	                fflush(stdin);
	                gets(M.end);
	            }
	
	            else if(op == 'D' || op == 'd'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Telefone: ");
	                fflush(stdin);
	                gets(M.tel);
	            }
	
	            else if(op == 'E' || op == 'e'){
					
					gotoxy(32, 12);
	                printf("Digite a nova Data de Admissao: ");
	                scanf("%d %d %d", &M.data.d, &M.data.m, &M.data.a);
	            }
	            
	            fwrite(&M, sizeof(TpMotoca), 1, Ptr);
				
				gotoxy(32, 14);
	            printf("Registro Alterado!");
	        }
    	}
    	
    	else{
	        gotoxy(32, 14);
	        printf("CPF invalido!");
	    }
    }

    fclose(Ptr);
    
    getch();
}

void AlterarPedido(void){
	
    LimpaTela();
    
    TpPedidos P;
    char op;
	
    FILE *Ptr = fopen("Pedidos.dat", "rb+");
	
    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro ao abrir o arquivo");
    }
	
    else{
    	
    	gotoxy(32, 10);
    	printf("----> ALTERAR PEDIDO <----");
		
		gotoxy(32, 12);
        printf("Digite o numero do pedido para alteracao de dados: ");
        scanf("%d", &P.num);

        int busca = BuscaSeqIndexPedido(Ptr, P.num);
		
        if(busca != -1){
        	
        	fseek(Ptr, busca, 0);
        	fread(&P, sizeof(TpPedidos), 1, Ptr);
        	
        	gotoxy(32, 14);
    		printf("Numero do Pedido: %d", P.num);
    		gotoxy(32, 15);
    		printf("Telefone do Cliente: %s", P.tel);
    		gotoxy(32, 16);
    		printf("Codigo da Pizza: %d", P.codpizza);
    		gotoxy(32, 17);
    		printf("CPF do Motoqueiro: %s", P.cpfmoto);
    		gotoxy(32, 18);
    		printf("Data do pedido: %d/%d/%d", P.datapedido.d, P.datapedido.m, P.datapedido.a);
    		gotoxy(32, 19);
    		printf("Situacao do Pedido: %d", P.situacao);
    		
    		gotoxy(32, 21);
    		printf("Continuar exclusao? (S/N): ");
    		op = toupper(getche());
    		
    		if(op == 'S'){
    			
    			LimpaTela();
    			
    			gotoxy(32, 10);
    			printf("----> ALTERAR PEDIDO <----");
    			
	    		fseek(Ptr, busca, 0);
				
				gotoxy(32, 12);
	            printf("O que deseja alterar?");
	            gotoxy(32, 13);
	            printf("[A] Numero");
	            gotoxy(32, 14);
	            printf("[B] Telefone");
	            gotoxy(32, 15);
	            printf("[C] Codigo da Pizza");
	            gotoxy(32, 16);
	            printf("[D] CPF do Motoqueiro");
	            gotoxy(32, 17);
	            printf("[E] Situacao");
	            gotoxy(32, 18);
	            printf("[F] Data do Pedido");
	            gotoxy(32, 20);
	            printf("Opcao: ");
	            fflush(stdin);
	            op = toupper(getche());
	            
	            LimpaTela();
	            gotoxy(32, 10);
	            printf("----> ALTERAR PEDIDO <----");
	
	            if(op == 'A' || op == 'a'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Numero: ");
	                scanf("%d", &P.num);
	            }
	
	            else if(op == 'B' || op == 'b'){
	
					gotoxy(32, 12);
	                printf("Digite o novo Telefone: ");
	                fflush(stdin);
	                gets(P.tel);
	            }
	
	            else if(op == 'C' || op == 'c'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Codigo da Pizza: ");
	                scanf("%d", &P.codpizza);
	            }
	
	            else if(op == 'D' || op == 'd'){
					
					gotoxy(32, 12);
	                printf("Digite o novo CPF do Motoqueiro: ");
	                fflush(stdin);
	                gets(P.cpfmoto);
	            }
	
	            else if(op == 'E' || op == 'e'){
					
					gotoxy(32, 12);
	                printf("Digite a nova Situacao do Pedido: ");
	                scanf("%d", &P.situacao);
	            }
	            
	            else if(op == 'F' || op == 'f'){
					
					gotoxy(32, 12);
	                printf("Digite a nova Data do Pedido: ");
	                scanf("%d %d %d", &P.datapedido.d, &P.datapedido.m, &P.datapedido.a);
	            }
	            
	            fwrite(&P, sizeof(TpPedidos), 1, Ptr);
				
				gotoxy(32, 14);
	            printf("Registro Alterado!");
    		}
        }
		
        else{
        	gotoxy(32, 14);
        	printf("Numero do pedido invalido!");
        }
    }
	
    fclose(Ptr);
    
    getch();
}

void AlterarPizza(void){

    LimpaTela();
    
    TpPizzas P;
    char op;

    FILE *Ptr = fopen("Pizzas.dat", "rb+");

    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro ao abrir o arquivo");
    }

    else{
    	
    	gotoxy(32, 10);
    	printf("----> ALTERAR PIZZA <----");
		
		gotoxy(32, 12);
        printf("Digite o codigo da pizza para alteracao de dados: ");
        scanf("%d", &P.cod);
		
        int busca = BuscaCod(Ptr, P.cod);
		
        if(busca != -1){
        	
        	fseek(Ptr, busca, 0);
        	fread(&P, sizeof(TpClientes), 1, Ptr);
        	
        	gotoxy(32, 14);
    		printf("Codigo da Pizza: %d", P.cod);
    		gotoxy(32, 15);
    		printf("Nome: %s", P.nome);
    		gotoxy(32, 16);
    		printf("Preco: %.2f", P.preco);
    		
    		gotoxy(32, 18);
    		printf("Continuar exclusao? (S/N): ");
    		op = toupper(getche());
    		
    		if(op == 'S'){
	    		
	    		LimpaTela();
	    		
	    		gotoxy(32, 10);
    			printf("----> ALTERAR PIZZA <----");
    			
    			fseek(Ptr, busca, 0);
				
				gotoxy(32, 12);
	            printf("O que deseja alterar?");
	            gotoxy(32, 13);
	            printf("[A] Nome");
	            gotoxy(32, 14);
	            printf("[B] Codigo");
	            gotoxy(32, 15);
	            printf("[C] Valor");
	            gotoxy(32, 17);
	            printf("Opcao: ");
	            fflush(stdin);
	            op = toupper(getche());
	            
	            LimpaTela();
	            gotoxy(32, 10);
	            printf("----> ALTERAR PIZZA <----");
	
	            if(op == 'A' || op == 'a'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Nome da Pizza: ");
	                fflush(stdin);
	                gets(P.nome);
	            }
	
	            else if(op == 'B' || op == 'b'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Codigo da Pizza: ");
	                scanf("%d", &P.cod);
	            }
	
	            else if(op == 'C' || op == 'c'){
					
					gotoxy(32, 12);
	                printf("Digite o novo Valor da Pizza: ");
	                scanf("%f", &P.preco);
	            }
	            
	            fwrite(&P, sizeof(TpPizzas), 1, Ptr);
				
				gotoxy(32, 14);
	            printf("Registro Alterado!");
    		}
        }
		
        else{
        	gotoxy(32, 14);
        	printf("CPF invalido!");
        }
    }
	
    fclose(Ptr);
    
    getch();
}

char MenuAlteracoes(void){
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> ALTERACOES <----");
	gotoxy(32, 11);
	printf("[A] Clientes");
	gotoxy(32, 12);
	printf("[B] Motoqueiros");
	gotoxy(32, 13);
	printf("[C] Pedidos");
	gotoxy(32, 14);
	printf("[D] Pizzas");
	gotoxy(32, 15);
	printf("[ESC] Voltar");
	gotoxy(32, 17);
	printf("Opcao: ");
	
	return toupper(getch());
}

void Alteracoes(void){
	
	char op;
	
	do{
		
		op = MenuAlteracoes();
		
		switch(op){
			
			case 'A': AlterarCliente();
					  break;
			
			case 'B': AlterarMotoca();
					  break;
			
			case 'C': AlterarPedido();
					  break;
			
			case 'D': AlterarPizza();
		}
		
	}while(op != 27);
}

void ExcFisicaCliente(void){
	
    TpClientes C;
    char auxtel[15];
	
    FILE *Ptr = fopen("Clientes.dat", "rb");
    
    LimpaTela();
	
    if(Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura\n");
    }
	
    else{
    	
    	gotoxy(32, 10);
    	printf("----> EXCLUSAO FISICA DE CLIENTE <----");
		
		gotoxy(32, 12);
        printf("Digite o telefone de quem deseja excluir: ");
        fflush(stdin);
        gets(auxtel);
		
        int busca = BuscaTel(Ptr, auxtel);
		
        if(busca != -1){
        	
        	fseek(Ptr, busca, 0);
        	fread(&C, sizeof(TpClientes), 1, Ptr);
        	
        	gotoxy(32, 14);
    		printf("Nome: %s", C.nome);
    		gotoxy(32, 15);
    		printf("Telefone: %s", C.tel);
    		gotoxy(32, 16);
    		printf("Endereco: %s", C.end);
    		gotoxy(32, 17);
    		printf("Cidade: %s", C.cid);
    		gotoxy(32, 18);
    		printf("CEP: %d", C.cep);
        	
        	gotoxy(32, 20);
        	printf("Confirmar exclusao?(S/N): ");
        	
        	if(toupper(getche()) == 'S'){
        		
        		FILE *PtrNovo = fopen("Temp.dat", "wb");
        		
        		fseek(Ptr, 0, 0);
        		
        		while(fread(&C, sizeof(TpClientes), 1, Ptr) != 0)
                   if(stricmp(C.tel, auxtel) != 0 && C.status == 'A')
                       fwrite(&C, sizeof(TpClientes), 1, PtrNovo);
                
                fclose(Ptr);
                fclose(PtrNovo);
                
                remove("Clientes.dat");
                rename("Temp.dat", "Clientes.dat");
                
                gotoxy(32, 22);
                printf("Registro excluido com sucesso.");
        	}
        }
		
        else{
        	
        	gotoxy(32, 14);
            printf("Telefone invalido!");
            fclose(Ptr);
        }
    	
    	fclose(Ptr);
    }
    
    fclose(Ptr);
    getch();
}

void ExcFisicaMotoca(void){

    TpMotoca M;
    char cpfaux[12], op;
	
    FILE *Ptr = fopen("Motoqueiros.dat", "rb+");
    
	LimpaTela();
	
    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura\n");
    }
	
    else{
    	
    	gotoxy(32, 10);
		printf("----> EXCLUSAO FISICA DE MOTOQUEIRO <----");
    	
    	gotoxy(32, 12);
        printf("Digite o CPF de quem deseja excluir: ");
        fflush(stdin);
        gets(cpfaux);
		
        int busca = BuscaSentCPFMotoca(Ptr, cpfaux);
		
        if(busca != -1){
        	
        	fseek(Ptr, busca, 0);
        	fread(&M, sizeof(TpMotoca), 1, Ptr);
        	
        	gotoxy(32, 14);
    		printf("Nome: %s", M.nome);
    		gotoxy(32, 15);
    		printf("Telefone: %s", M.tel);
    		gotoxy(32, 16);
    		printf("Endereco: %s", M.end);
    		gotoxy(32, 17);
    		printf("CPF: %s", M.cpf);
    		gotoxy(32, 18);
    		printf("Data de admissao: %d/%d/%d", M.data.d, M.data.m, M.data.a);
        	
        	gotoxy(32, 20);
        	printf("Confirmar exclusao?(S/N): ");
        	fflush(stdin);
        	op = toupper(getche());
        	
        	if(op == 'S'){
        		
        		FILE *PtrNovo = fopen("temp.dat", "wb");
        		
        		fseek(Ptr, 0, 0);
        		
        		while (fread(&M, sizeof(TpMotoca), 1, Ptr) != 0)
                   if(stricmp(M.cpf, cpfaux) != 0 && M.status == 'A')
                       fwrite(&M, sizeof(TpMotoca), 1, PtrNovo);
            	
                fclose(Ptr);
                fclose(PtrNovo);
                
                remove("Motoqueiros.dat");
                rename("temp.dat", "Motoqueiros.dat");
                
                gotoxy(32, 22);
                printf("Registro excluido com sucesso.");
        	}
        	
        	else
        		fclose(Ptr);
        }
		
        else{
        	gotoxy(32, 14);
        	printf("CPF invalido!");
        }
    }
	
    fclose(Ptr);
    
    getch();
}

void ExcFisicaPedido(void){

    TpPedidos P;
    int numaux;
    char op;
    
    LimpaTela();
    
    FILE *Ptr = fopen("Pedidos.dat", "rb");
	
    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura\n");
    }

    else{
    	
    	gotoxy(32, 10);
    	printf("----> EXCLUSAO FISICA DE PEDIDO <----");
    	
    	gotoxy(32, 12);
        printf("Digite o numero do pedido que deseja excluir: ");
        scanf("%d", &numaux);
		
        int busca = BuscaBinariaPedido(Ptr, numaux);
		
        if(busca != -1){
        	
        	fseek(Ptr, busca, 0);
        	fread(&P, sizeof(TpPedidos), 1, Ptr);
        	
        	gotoxy(32, 14);
    		printf("Numero do Pedido: %d", P.num);
    		gotoxy(32, 15);
    		printf("Telefone do Cliente: %s", P.tel);
    		gotoxy(32, 16);
    		printf("Codigo da Pizza: %d", P.codpizza);
    		gotoxy(32, 17);
    		printf("CPF do Motoqueiro: %s", P.cpfmoto);
    		gotoxy(32, 18);
    		printf("Data do pedido: %d/%d/%d", P.datapedido.d, P.datapedido.m, P.datapedido.a);
    		gotoxy(32, 19);
    		printf("Situacao do Pedido: %d", P.situacao);
        	
        	gotoxy(32, 21);
        	printf("Confirmar exclusao?(S/N): ");
        	fflush(stdin);
        	op = toupper(getche());
        	
        	if(op == 'S'){
        		
        		FILE *PtrNovo = fopen("temp.dat", "wb");
        		
        		fseek(Ptr, 0, 0);
			
				while (fread(&P, sizeof(TpPedidos), 1, Ptr) != 0)
                   if(P.num != numaux && P.status == 'A')
                       fwrite(&P, sizeof(TpPedidos), 1, PtrNovo);
            
                fclose(Ptr);
                fclose(PtrNovo);
                
                remove("Pedidos.dat");
                rename("temp.dat", "Pedidos.dat");
                
                gotoxy(32, 23);
                printf("Registro excluido com sucesso.");
			}
			
			else
				fclose(Ptr);
        }
		
        else{
        	gotoxy(32, 14);
        	printf("Numero do pedido invalido!");
        }
        
        fclose(Ptr);
    }

    fclose(Ptr);
    
    getch();
}

void ExcFisicaPizza(void){

    TpPizzas P;
    int codaux;
    char op;
    
    LimpaTela();

    FILE *Ptr = fopen("Pizzas.dat", "rb");
	
    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura\n");
    }
	
    else{
    	
    	gotoxy(32, 10);
    	printf("----> EXCLUSAO FISICA DE PIZZA <----");
    	
    	gotoxy(32, 12);
        printf("Digite o codigo da pizza que deseja excluir: ");
        scanf("%d", &codaux);

        int busca = BuscaCod(Ptr, codaux);

        if(busca != -1){
        	
        	fseek(Ptr, busca, 0);
        	fread(&P, sizeof(TpPizzas), 1, Ptr);
        	
        	gotoxy(32, 14);
    		printf("Codigo da Pizza: %d", P.cod);
    		gotoxy(32, 15);
    		printf("Nome: %s", P.nome);
    		gotoxy(32, 16);
    		printf("Preco: %.2f", P.preco);
        	
        	gotoxy(32, 18);
        	printf("Confirmar exclusao?(S/N): ");
        	fflush(stdin);
        	op = toupper(getche());
        	
        	if(op == 'S'){
        		
        		FILE *PtrNovo = fopen("temp.dat", "wb");
        		
        		fseek(Ptr, 0, 0);
        		
        		while (fread(&P, sizeof(TpPizzas), 1, Ptr) != 0)
                   if(P.cod != codaux && P.status == 'A')
                       fwrite(&P, sizeof(TpPizzas), 1, PtrNovo);
            	
                fclose(Ptr);
                fclose(PtrNovo);
                
                remove("Pizzas.dat");
                rename("temp.dat", "Pizzas.dat");
                
                gotoxy(32, 20);
                printf("Registro excluido com sucesso.");
        	}
        	
			else
        		fclose(Ptr);
        }
		
        else{
        	gotoxy(32, 14);
        	printf("Codigo invalido!");
        }
        
        fclose(Ptr);
    }
	
    fclose(Ptr);
    
    getch();
}

char MenuExclusoesFisicas(void){
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> EXCLUSOES FISICAS <----");
	gotoxy(32, 11);
	printf("[A] Clientes");
	gotoxy(32, 12);
	printf("[B] Motoqueiros");
	gotoxy(32, 13);
	printf("[C] Pedidos");
	gotoxy(32, 14);
	printf("[D] Pizzas");
	gotoxy(32, 15);
	printf("[ESC] Voltar");
	gotoxy(32, 17);
	printf("Opcao: ");
	
	return toupper(getch());
}

void ExclusoesFisicas(void){
	
	char op;
	
	do{
		
		op = MenuExclusoesFisicas();
		
		switch(op){
			
			case 'A': ExcFisicaCliente();
					  break;
			
			case 'B': ExcFisicaMotoca();
					  break;
			
			case 'C': ExcFisicaPedido();
					  break;
			
			case 'D': ExcFisicaPizza();
		}
		
	}while(op != 27);
}

void ExcLogicaCliente(void){

    TpClientes C;
    char auxtel[15];
    
    FILE *Ptr = fopen("Clientes.dat", "rb+");
    
    LimpaTela();
    
    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura!\n");
    }

    else{
    	
    	gotoxy(32, 10);
    	printf("----> EXCLUSAO LOGICA DE CLIENTE <----");
		
		gotoxy(32, 12);
        printf("Digite o telefone de quem deseja excluir: ");
        fflush(stdin);
        gets(auxtel);

        int busca = BuscaTel(Ptr, auxtel);
        
        if(busca != -1){
        	
        	if(C.status == 'I'){
        		gotoxy(32, 14);
        		printf("Cliente ja inativo!");
        	}
        	
        	else{
        		
        		fseek(Ptr, busca, 0);
	        	fread(&C, sizeof(TpClientes), 1, Ptr);
	        	
	        	gotoxy(32, 14);
	    		printf("Nome: %s", C.nome);
	    		gotoxy(32, 15);
	    		printf("Telefone: %s", C.tel);
	    		gotoxy(32, 16);
	    		printf("Endereco: %s", C.end);
	    		gotoxy(32, 17);
	    		printf("Cidade: %s", C.cid);
	    		gotoxy(32, 18);
	    		printf("CEP: %d", C.cep);
				
				gotoxy(32, 20);
	            printf("Confirmar exclusao? (S/N): ");
	
	            if(toupper(getche()) == 'S'){
	                
	                C.status = 'I';
	                
	                fseek(Ptr, busca, 0);
	                fwrite(&C, sizeof(TpClientes), 1, Ptr);
					
					gotoxy(32, 22);
	                printf("Registro excluido com sucesso!");
	            }
        	}
        }
		
        else{
        	gotoxy(32, 14);
        	printf("Telefone invalido!");
        }
        
        fclose(Ptr);
    }
    
    getch();
}

void ExcLogicaMotoca(void){
	
    TpMotoca M;
    char cpfaux[12];
    
    FILE *Ptr = fopen("Motoqueiros.dat", "rb+");
    
    LimpaTela();
    
    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura!\n");
    }
    
    else{
    	
    	gotoxy(32, 10);
    	printf("----> EXCLUSAO LOGICA DE MOTOQUEIRO <----");
		
		gotoxy(32, 12);
        printf("Digite o CPF de quem deseja excluir: ");
        fflush(stdin);
        gets(cpfaux);

        int busca = BuscaSentCPFMotoca(Ptr, cpfaux);

        if(busca != -1){
        	
        	if(M.status == 'I'){
        		gotoxy(32, 14);
        		printf("Motoqueiro ja inativo!\n");
        	}
				
			else{
				
				fseek(Ptr, busca, 0);
	        	fread(&M, sizeof(TpMotoca), 1, Ptr);
	        	
	        	gotoxy(32, 14);
	    		printf("Nome: %s", M.nome);
	    		gotoxy(32, 15);
	    		printf("Telefone: %s", M.tel);
	    		gotoxy(32, 16);
	    		printf("Endereco: %s", M.end);
	    		gotoxy(32, 17);
	    		printf("CPF: %s", M.cpf);
	    		gotoxy(32, 18);
	    		printf("Data de admissao: %d/%d/%d", M.data.d, M.data.m, M.data.a);
				
				gotoxy(32, 20);
				printf("Confirmar exclusao? (S/N): ");
            
	            if(toupper(getche()) == 'S'){
	                        
	                M.status = 'I';
	                
	                fseek(Ptr, busca, 0);
	                fwrite(&M, sizeof(TpMotoca), 1, Ptr);
	                
	                gotoxy(32, 22);
	                printf("Registro excluido com sucesso!");
	            }
			}
        }
        
        else{
        	gotoxy(32, 14);
        	printf("CPF invalido!");
        }
        
        fclose(Ptr);
    }
    
    getch();
}

void ExcLogicaPedido(void){

    TpPedidos P;
    
    FILE *Ptr = fopen("Pedidos.dat", "rb+");
    
    LimpaTela();
    
    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura!");
    }
	
    else{
    	
    	gotoxy(32, 10);
    	printf("----> EXCLUSAO LOGICA DE PEDIDO <----");
		
		gotoxy(32, 12);
        printf("Digite o numero do pedido que deseja excluir: ");
        scanf("%d", P.num);

        int busca = BuscaSeqIndexPedido(Ptr, P.num);

        if(busca != -1){
        	
        	if(P.status == 'I'){
        		gotoxy(32, 14);
        		printf("Pedido ja inativo!");
        	}
			
			else{
				
				fseek(Ptr, busca, 0);
        		fread(&P, sizeof(TpPedidos), 1, Ptr);
				
				gotoxy(32, 14);
	    		printf("Numero do Pedido: %d", P.num);
	    		gotoxy(32, 15);
	    		printf("Telefone do Cliente: %s", P.tel);
	    		gotoxy(32, 16);
	    		printf("Codigo da Pizza: %d", P.codpizza);
	    		gotoxy(32, 17);
	    		printf("CPF do Motoqueiro: %s", P.cpfmoto);
	    		gotoxy(32, 18);
	    		printf("Data do pedido: %d/%d/%d", P.datapedido.d, P.datapedido.m, P.datapedido.a);
	    		gotoxy(32, 19);
	    		printf("Situacao do Pedido: %d", P.situacao);
				
				gotoxy(32, 21);
				printf("Confirmar exclusao? (S/N): ");
            
	            fseek(Ptr, busca, 0);
		        fread(&P, sizeof(TpPedidos), 1, Ptr);
	
	            if(toupper(getche()) == 'S'){
	                        
	                P.status = 'I';
	                
	                fseek(Ptr, busca, 0);
	                fwrite(&P, sizeof(TpPedidos), 1, Ptr);
					
					gotoxy(32, 23);
	                printf("Registro excluido com sucesso!");
            	}
			}
        }
        
        else{
        	gotoxy(32, 14);
        	printf("Numero invalido!");
        }
    	
    	fclose(Ptr);
    }
    
    getch();
}

void ExcLogicaPizza(void){

    TpPizzas P;
    
    FILE *Ptr = fopen("Pizzas.dat", "rb+");
    
    LimpaTela();
    
    if (Ptr == NULL){
    	gotoxy(32, 10);
    	printf("Erro de abertura!");
    }
	
    else{
    	
    	gotoxy(32, 10);
    	printf("----> EXCLUSAO LOGICA DE PIZZA <----");
		
		gotoxy(32, 12);
        printf("Digite o codigo da pizza que deseja excluir: ");
        scanf("%d", P.cod);

        int busca = BuscaCod(Ptr, P.cod);

        if(busca != -1){
        	
        	if(P.status == 'I'){
        		gotoxy(32, 14);
        		printf("Pizza ja inativa!");
        	}
				
			else{
				
				fseek(Ptr, busca, 0);
	        	fread(&P, sizeof(TpPizzas), 1, Ptr);
	        	
	        	gotoxy(32, 14);
	    		printf("Codigo da Pizza: %d", P.cod);
	    		gotoxy(32, 15);
	    		printf("Nome: %s", P.nome);
	    		gotoxy(32, 16);
	    		printf("Preco: %.2f", P.preco);
				
				gotoxy(32, 18);
				printf("Confirmar exclusao? (S/N): ");

	            if(toupper(getche()) == 'S'){
	                
	                P.status = 'I';
	                
	                fseek(Ptr, busca, 0);
	                fwrite(&P, sizeof(TpPizzas), 1, Ptr);
	            	
	            	gotoxy(32, 20);
	                printf("Registro excluido com sucesso!");
            	}
			}
        }
		
        else{
        	gotoxy(32, 14);
        	printf("\nCodigo invalido!");
        }
        
    	fclose(Ptr);
    }
    
    getch();
}

char MenuExclusoesLogicas(void){
	
	LimpaTela();
	
	gotoxy(32, 10);
	printf("----> EXCLUSOES LOGICAS <----");
	gotoxy(32, 11);
	printf("[A] Clientes");
	gotoxy(32, 12);
	printf("[B] Motoqueiros");
	gotoxy(32, 13);
	printf("[C] Pedidos");
	gotoxy(32, 14);
	printf("[D] Pizzas");
	gotoxy(32, 15);
	printf("[ESC] Voltar");
	gotoxy(32, 17);
	printf("Opcao: ");
	
	return toupper(getch());
}

void ExclusoesLogicas(void){
	
	char op;
	
	do{
		
		op = MenuExclusoesLogicas();
		
		switch(op){
			
			case 'A': ExcLogicaCliente();
					  break;
			
			case 'B': ExcLogicaMotoca();
					  break;
			
			case 'C': ExcLogicaPedido();
					  break;
			
			case 'D': ExcLogicaPizza();
		}
		
	}while(op != 27);
}

void Moldura(int CI, int LI, int CF, int LF, int Frente){
	
	textcolor(Frente);
	//textbackground(Fundo);
	
	gotoxy(CI,LI);
	printf("%c", 201);
	gotoxy(CF,LI);
	printf("%c", 187);
	gotoxy(CI,LF);
	printf("%c", 200);
	gotoxy(CF,LF);
	printf("%c", 188);
	
	for(int a=CI+1; a<CF; a++){
		gotoxy(a, LI);
		printf("%c", 205);
		gotoxy(a, LF);
		printf("%c", 205);
	}
	
	for(int a=LI+1; a<LF; a++){
		gotoxy(CI, a);
		printf("%c", 186);
		gotoxy(CF, a);
		printf("%c", 186);
	}
	
	textcolor(7);
	//textbackground(0);
}

void FormPrincipal(void){
	
	system("cls");
	
	Moldura(10, 5, 110, 29, 15); // moldura externa
	
	Moldura(11, 6, 109, 8, 4); // moldura do titulo
	
	gotoxy(54, 7);
	printf("### PIZZARIA ###");
	
	Moldura(11, 9, 30, 28, 4); // moldura do menu
	
	Moldura(31, 9, 109, 28, 4); // moldura do jogo
}

char Menu (void){
	
	system("cls");
	
	FormPrincipal();
	
	gotoxy(12, 10);
	printf(" ----> MENU <----\n\n");
	gotoxy(12, 11);
	printf("[A] Cadastros");
	gotoxy(12, 12);
	printf("[B] Relatorios");
	gotoxy(12, 13);
	printf("[C] Consultas");
	gotoxy(12, 14);
	printf("[D] Alteracoes");
	gotoxy(12, 15);
	printf("[E] Exclusoes Fis.");
	gotoxy(12, 16);
	printf("[F] Exclusoes Log.");
	gotoxy(12, 17);
	printf("[ESC] Sextou");
	gotoxy(12, 19);
	printf("Opcao: ");
	
	return toupper(getche());
}

int main(void){
	
	char op;
	
	CriarArq();
	
	do{
	
		op = Menu();
		
		switch (op){
			
			case 'A': Cadastros();
					  break;
					  
			case 'B': Relatorios();
					  break;
					  
			case 'C': Consultas();
					  break;
					  
			case 'D': Alteracoes();
					  break;
			
			case 'E': ExclusoesFisicas();
					  break;
			
			case 'F': ExclusoesLogicas();
		}
		
	}while(op != 27);	
	
	return 0;
}

// GRUPO: IGOR CARBONARI SILVA, EDUARDO CARBONARI SILVA E VINICIUS DO NASCIMENTO ROCHA
