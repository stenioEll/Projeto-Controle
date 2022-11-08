/*
 * Declaração dos pinos do sensor ultrasonico
 */
#define trigPin 8
#define echoPin 9

/*
 * Declaração dos pinos do motor 
 * enB      -> Pino para habilitar/desabilitara o motor e controlar sua velocidade
 * in3/in2  -> são utilizados para controlar o sentido de giro do motor
 * 
 * IN3 | IN4 | Sentido de Giro
 * 0   | 0   | Motor desligado
 * 1   | 0   | Motor para frente
 * 0   | 1   | Motor para trás
 * 1   | 1   | Motor desligado
 */
int enB = 3;
int in3 = 2;
int in4 = 7;

/*
 * Distâncias de referência
 * start -> posição inicial do trem
 * stop1 -> primeira parada do trem em direção ao sensor ultrasonico
 * stop2 -> segunda parada do trem em direção ao sensor ultrasonico
 * stop3 -> terceira parada do trem em direção ao sensor ultrasonico
 * 
 * Para alterar qualquer valor de referência descrito abaixo, basta alterar
 * na variável correspondente.
 */
int start = 50;
int stop1 = 30;
int stop2 = 20;
int stop3 = 10;

/*
 * Variável para registro de distância do trem ao sensor ultrasonico
 */
float distance;

void setup() {
  /*
   * Configurações dos pinos do sensor
   */
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  /*
   * Configurações dos pinos do L293D
   */
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  /*
   * Inicializa o motor desligado
   */
  stopMotor();

  /*
   * Configura a frequência da comunicação serial
   */
  Serial.begin(9600); 
}

void loop() {
  //Atualiza o valor da distância
  distance = calculateDistance();

  //Caso deseje acompanhar a distância, mantenha o print
  //Caso deseje acompanhar a velocidade, comente esta linha
  Serial.println(distance);

  /*
   * Se o trem ainda não estiver chegado na primeira parada, 
   * então ele segue se movendo em direção a ela.
   * O motor será movido com a velocidade correspondente do
   * resultado do cálculo se baseando na distância atual e
   * da distância para o destino, no caso a primeira parada
   * (stop1).
   * Ao se atingir a primeira parada, para o motor por 3s.
   */
  if(distance >= stop1){
    moveMotor(getSpeed(distance, stop1, start));
    if(distance <= stop1){
      stopMotor();
      delay(3000);
    }
  }
  /*
   * Se o trem ainda não estiver chegado na segunda parada, 
   * então ele segue se movendo em direção a ela.
   * O motor será movido com a velocidade correspondente do
   * resultado do cálculo se baseando na distância atual e
   * da distância para o destino, no caso a segunda parada
   * (stop2).
   * Ao se atingir a segunda parada, para o motor por 3s.
   */
  else if(distance >= stop2){
    moveMotor(getSpeed(distance, stop2, stop1));
    if(distance <= stop2){
      stopMotor();
      delay(3000);
    }
  }
  /*
   * Se o trem ainda não estiver chegado na terceira parada, 
   * então ele segue se movendo em direção a ela.
   * O motor será movido com a velocidade correspondente do
   * resultado do cálculo se baseando na distância atual e
   * da distância para o destino, no caso a terceira parada
   * (stop3).
   * Ao se atingir a terceira parada, o motor é desligado.
   */
  else if(distance >= stop3){
    moveMotor(getSpeed(distance, stop3, stop2));
    if(distance <= stop3){
      stopMotor();
    }
  }
}

/*
 * Essa função se responsabiliza por disparar um sinal sonoro
 * do gatilho (trigger) e aguardar o echo receber o retorno da
 * onda sonora disparada. Baseando-se na velocidade do som, 
 * calcula-se a distância aproximada.
 */
float calculateDistance(){
  // Garante o pino trigger como baixa tensão
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Dispara o sinal sonoro por 10 microsecundos
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Calcula o tempo para que o echo receba o retorno do sinal sonoro
  long duration = pulseIn(echoPin, HIGH);

  // Divide o tempo para o sinal sonoro chegar no echo pela velocidade da
  // luz e estima-se a distância
  return duration*0.034/2;
}

/*
 * Move o motor para frente com uma velocidade definida em w
 * por 50 milisegundos e atualiza o valor da distancia.
 */
void moveMotor(int w){
  analogWrite(enB, w);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(50);

  distance = calculateDistance();
}

/*
 * Desliga o motor
 */
void stopMotor(){
  analogWrite(enB, 0);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

/*
 * Calcula a velocidade se baseando na distância atual.
 * É uma função simples, recomendo que se mudem a função
 * tomando por base a modelagem de controle e a base de
 * cálculo utilizada.
 * Desta forma, quando a distância for a mínima (10cm), 
 * garante que a velocidade do dispositivo seja 40 pwm,
 * enquanto que quando a distância for a máxima (50cm)
 * garante que a velocidade do dispositivo seja 200 pwm.
 * É importante garantir que a velocidade não chegue a
 * 0 pwm pois, nesse caso, antes que chegasse em 0 a 
 * velocidade estaria baixa de tal forma que o motor
 * não teria força suficiente para chegar no destino
 * final.
 */
int getSpeed(float currentDistance, float focusDistance, float lastDistance){
  int v = int((abs((currentDistance - focusDistance)/(lastDistance - focusDistance)))*(40))+40;

  //Caso deseje acompanhar a velocidade, descomente esta linha
  //Caso deseje acompanhar a distância, mantenha esta linha comentada
  //Serial.println(v);

  return v;
}
