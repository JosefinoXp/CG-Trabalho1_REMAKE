/*
    ESSE PRIMEIRO CÓDIGO FOI DESENVOLVIDO 
    COM O INTUITO DE SUPRIR O
    ESCOPO DA PRIMEIRA ATIVIDADE
 */


package Atividade1;
import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.List;

public class ProgramaDesenho {
    private static PintarTela areaPintura = new PintarTela();

    public static void main(String[] args) {
        JFrame frame = new JFrame("ProgramaDesenho");
        frame.setSize(600, 600);
        frame.setLocation(500,500);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setLocationRelativeTo(null);


        JPanel controlPanel = new JPanel();
        JButton limparBotao = criarBotao("Limpar Tela", Color.BLACK);
        JButton[] formatos = {
            criarBotao("Reta", new Color(63, 81, 181)),
            criarBotao("Retangulo", new Color(142, 68, 173)),
            criarBotao("Circulo", new Color(39, 174, 96)),
            criarBotao("Livre", new Color(70, 120, 50) )
        };

        controlPanel.add(limparBotao);
        
        for (JButton botao : formatos) {
            controlPanel.add(botao);
        }

        frame.add(controlPanel, BorderLayout.NORTH);
        frame.add(areaPintura, BorderLayout.CENTER);

        limparBotao.addActionListener(e -> areaPintura.LimparTela());
        
        for (int i = 0; i < formatos.length; i++) {
            final TipoFormato tipo = TipoFormato.values()[i];
            formatos[i].addActionListener(e -> areaPintura.setFormatoAtual(tipo));
        }

        frame.setVisible(true);
    }

    //Template de Botão
    private static JButton criarBotao(String texto, Color cor) {
        JButton botao = new JButton(texto);
        botao.setBackground(cor);
        botao.setForeground(cor.WHITE);
        botao.setFocusPainted(false);
        botao.setBorderPainted(false);
        return botao;
    }

    static class PintarTela extends JPanel {
        private List<Formato> formatos;
        private Color corAtual;
        private TipoFormato formatoAtual;

        public PintarTela() {

            formatos = new ArrayList<>();
            corAtual = Color.BLACK;
            formatoAtual = TipoFormato.PONTO;
            setBackground(Color.WHITE);

            addMouseListener(new MouseAdapter() {
                
                public void mousePressed(MouseEvent e) {
                    Point comeco = e.getPoint();
                    Formato formato;
                    switch (formatoAtual) {
                        case PONTO:
                            formato = new FormatoLivre(corAtual, comeco);
                            break;

                        case RETA:
                            formato = new FormatoReta(corAtual, comeco);
                            break;

                        case RETANGULO:
                            formato = new FormatoRetangulo(corAtual, comeco);
                            break;

                        case CIRCULO:
                            formato = new FormatoCirculo(corAtual, comeco);
                            break;

                        default:
                            formato = null;
                            break;
                    }

                    formatos.add(formato);
                }

            });


            addMouseMotionListener(new MouseAdapter() {
                
                public void mouseDragged(MouseEvent e) {
                    if (!formatos.isEmpty()) {
                        Point fim = e.getPoint();
                        formatos.get(formatos.size() - 1).addPoint(fim);
                        repaint();
                    }
                }

            });



        }

        public void setCorAtual(Color cor) {
            corAtual = cor;
        }

        public Color getCorAtual() {
            return corAtual;
        }

        public void setFormatoAtual(TipoFormato tipoFormato) {
            formatoAtual = tipoFormato;
        }

        public TipoFormato getFormatoAtual() {
            return formatoAtual;
        }

        public void LimparTela() {
            formatos.clear();
            repaint();
        }

        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            for(Formato formato : formatos) {
                formato.draw(g);
            }
        }
        
    }

    interface Formato {
        void draw(Graphics g);
        void addPoint(Point point);
    }

    enum TipoFormato
    { RETA, RETANGULO, CIRCULO, PONTO };

    public static class FormatoLivre implements Formato {

        private Color cor;
        private List<Point> pontos;

        public FormatoLivre(Color cor, Point comeco) {
            this.cor = cor;

            pontos = new ArrayList<>();
            pontos.add(comeco);
        }

        @Override
        public void draw(Graphics g) {
            g.setColor(cor);
            for(int i = 1; i < pontos.size(); i++) {
                Point comeco = pontos.get(i - 1);
                Point fim = pontos.get(i);
                g.drawLine(comeco.x, comeco.y, fim.x, fim.y);
            }
        }

        @Override
        public void addPoint(Point point) {
            pontos.add(point);
        }
    
    }

    static class FormatoReta implements Formato {
        private Color cor;
        private List<Point> pontos;

        public FormatoReta(Color cor, Point comeco) {
            this.cor = cor;

            pontos = new ArrayList<>();
            pontos.add(comeco);
        }
        
        @Override
        public void draw(Graphics g) {
            g.setColor(cor);
            if (pontos.size() >= 2) {
                Point comeco = pontos.get(0);
                Point fim = pontos.get(pontos.size() - 1);
                g.drawLine(comeco.x, comeco.y, fim.x, fim.y);
            }
        }
        @Override
        public void addPoint(Point point) {
            if (pontos.size() > 1) {
                pontos.set(1, point);
            } else {
                pontos.add(point);
            }
        }

        
    }

    static class FormatoRetangulo implements Formato {

        private Color cor;
        private Point comeco;
        private Point fim;

        public FormatoRetangulo(Color cor, Point comeco) {
            this.cor = cor;
            this.comeco = comeco;
            this.fim = comeco;
        }

        @Override
        public void draw(Graphics g) {
            g.setColor(cor);
            int x = Math.min(comeco.x, fim.x);
            int y = Math.min(comeco.y, fim.y);
            int largura = Math.abs(comeco.x - fim.x);
            int altura = Math.abs(comeco.y - fim.y);
            g.drawRect(x, y, largura, altura);
        }

        @Override
        public void addPoint(Point point) {
            fim = point;
        }
        
    }

    static class FormatoCirculo implements Formato {

        private Color cor;
        private Point comeco;
        private Point fim;

        public FormatoCirculo(Color cor, Point comeco) {
            this.cor = cor;
            this.comeco = comeco;
            this.fim = comeco;
        }

        @Override
        public void draw(Graphics g) {
            g.setColor(cor);
            int x = Math.min(comeco.x, fim.x);
            int y = Math.min(comeco.y, fim.y);
            int largura = Math.abs(comeco.x - fim.x);
            g.drawOval(x, y, largura, largura);
        }

        @Override
        public void addPoint(Point point) {
            fim = point;
        }

    }

}