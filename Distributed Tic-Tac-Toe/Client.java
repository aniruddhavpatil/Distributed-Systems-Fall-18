
import java.rmi.registry.*;
import java.util.Scanner;

public class Client {

    private static String checkBoard(String board){
        String [] arrayBoard = board.split("");
        for (int i = 0; i < 8; i++) {
            if(arrayBoard[victoryPositions[i][0]].equals(arrayBoard[victoryPositions[i][1]]))
                if(arrayBoard[victoryPositions[i][1]].equals(arrayBoard[victoryPositions[i][2]])) return "WON_" + arrayBoard[victoryPositions[i][0]];
        }

        for (int i = 0; i < 9; i++) {
            boolean truthVal = arrayBoard[i].matches("\\d");
//            System.out.println(arrayBoard[i].charAt(0) + " " + truthVal);
            if(truthVal) return "RUNNING_RUNNING";
        }

        return "DRAW_DRAW";
    }
    private static void printboard(String sboard){
        String [] board = sboard.split("");
        System.out.println(board[0] + " " + board[1] + " " + board[2] + "\n" + board[3] + " " + board[4] + " " + board[5] + "\n" + board[6] + " " + board[7] + " " + board[8]);
    }

    private static int[][] victoryPositions = new int[][]{
            {0,3,6},
            {1,4,7},
            {2,5,8},

            {0,4,8},
            {2,4,6},

            {0,1,2},
            {3,4,5},
            {6,7,8}
    };

    public static void main(String[] args) {
        try {
            String board;
            String gamestatus;
            int playernumber;
            String text;
            String checkedGameStatus;
            int usernumber;
            Scanner scn = new Scanner(System.in);
            int turn;
            int gamenumber;
            String status;



            Registry registry = LocateRegistry.getRegistry(null);
            intfc stub = (intfc) registry.lookup("TicTacToe");

            System.out.println("Enter username");
            text = scn.nextLine();
            usernumber = stub.addusername(text);
            System.out.println("Waiting for game");

            while(true){
                status = stub.getstatus(usernumber);
                if(status.contains("Ready")){

                    playernumber = Integer.valueOf(status.split(" ")[1]);
                    int playerid = playernumber + 1;
                    System.out.println("Found a game");
                    System.out.println("Your Player ID is " + playerid);
                    break;
                }
                else if(status.contentEquals("Waiting")){
                    continue;
                }
            }


            gamestatus = stub.getgamestatus(usernumber);
            String playerString = gamestatus.split(" ")[0];
            String gameString = gamestatus.split(" ")[1];
            playernumber = Integer.valueOf(playerString);
            gamenumber = Integer.valueOf(gameString);
            System.out.println("Game started");
            System.out.println("You are player " + playernumber + " assigned to game " + gamenumber);


            while(true) {
                char sign;
                if (playernumber < 2) sign = 'X';
                else sign = 'O';
                String boardString = stub.getgameboardstatus(gamenumber);
                board = boardString.split("_")[0];
                turn = Integer.valueOf(boardString.split("_")[1]);

                checkedGameStatus = checkBoard(board);
                String victoryString = checkedGameStatus.split("_")[0];
                String vic1 = checkedGameStatus.split("_")[1];

                boolean victoryCondition = victoryString.equals("WON");
                if(victoryCondition){
                    if(vic1.charAt(0) == sign) System.out.println("You won");
                    else System.out.println("You lost");
                    System.out.println("Would you like to play again? y/n");
                    String response = scn.nextLine();
                    if(response.contentEquals("y")){
                        stub.restart(usernumber, gamenumber);
                        while(stub.getstatus(usernumber).contentEquals("Restart"));
                        String stubstat = stub.getstatus(usernumber);
                        if(stubstat.contentEquals("Left")){
                            break;
                        }
                        String boardstate = stub.getgameboardstatus(gamenumber);
                        board = boardstate.split("_")[0];
                        turn = Integer.valueOf(boardstate.split("_")[1]);
                    }
                    else{
                        break;
                    }
                }
                else if(checkedGameStatus.split("_")[0].equals("DRAW")){
                    String restart = "Restart";
                    String left = "Left";
                    System.out.println("DRAW");
                    System.out.println("Would you like to play again? y/n");
                    String response = scn.nextLine();
                    if(response.contentEquals("y")){
                        stub.restart(usernumber, gamenumber);
                        while(stub.getstatus(usernumber).contentEquals(restart));
                        if(stub.getstatus(usernumber).contentEquals(left)){
                            break;
                        }
                        String boardstate = stub.getgameboardstatus(gamenumber);
                        board = boardstate.split("_")[0];
                        turn = Integer.valueOf(boardstate.split("_")[1]);
                    }
                    else{
                        break;
                    }
                }

                if (playernumber - 1 == turn) {
                    String move;
                    printboard(board);
                    int index = -1;
                    System.out.println("Input your move");
                    while(true) {
                        move = scn.nextLine();
                        if(move.matches("\\d")) {
                            index = Integer.valueOf(move) - 1;
                            if (index > -1){
                                if(index < 9){
                                    if(board.charAt(index) != 'X' && board.charAt(index) != 'O') break;
                                    else System.out.println("Tile used already");
                                }
                            }
                            else System.out.println("Please provide number [1-9] on a tile not taken");
                        }
                        else{
                            System.out.println("Please provide number [1-9]");
                        }

                    }

                    StringBuilder updatedboard = new StringBuilder(board);
                    updatedboard.setCharAt(index, sign);
                    int newTurn = (turn + 1) % 2;
                    stub.updategameboardstatus(gamenumber, updatedboard + "_" + newTurn);

                } else {
                    System.out.println("Wait for your turn");
                    while (true) {
                        String boardStatus = stub.getgameboardstatus(gamenumber);
                        int play = playernumber - 1;
                        if (play == Integer.valueOf(boardStatus.split("_")[1])) break;
                    }
                }
            }
            stub.exit(usernumber,gamenumber);
            System.out.println("Leaving");

        } catch (Exception e) {
            e.printStackTrace();
            System.err.println("Client exception: " + e.toString());
        }
    }
}