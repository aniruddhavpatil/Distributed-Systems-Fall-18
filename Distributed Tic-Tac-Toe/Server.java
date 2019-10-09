import java.rmi.RemoteException;
import java.rmi.Remote;
import java.util.*;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;


public class Server extends remoteobj {
    public Server() {}
    public static void main(String args[]) {
        try {

            remoteobj obj = new remoteobj();
            intfc stub = (intfc) UnicastRemoteObject.exportObject(obj, 0);
            Registry registry = LocateRegistry.getRegistry();
            registry.bind("TicTacToe", stub);
            System.err.println("Server ready");
        } catch (Exception e) {
            System.err.println("Server exception: " + e.toString());
            e.printStackTrace();
        }
    }
}


// Creating Remote interface for our application
interface intfc extends Remote {
//    void removeusername(String name, int usernumber) throws RemoteException;

    String getstatus(int usernumber) throws RemoteException;

    String getgameboardstatus(int gamenumber) throws RemoteException;

    void exit(int usernumber, int gamenumber) throws RemoteException;

    int addusername(String name) throws RemoteException;

    String getgamestatus(int usernumber) throws RemoteException;

    void restart(int usernumber, int gamenumber) throws RemoteException;

//    void creategame(int user1, int user2, int gamenumber) throws RemoteException;

    void updategameboardstatus(int gamenumber, String gameboardstatus) throws RemoteException;

}

class remoteobj implements intfc {

    Map<Integer, String> games = new HashMap<>();
    Map<Integer, String> gameboard = new HashMap<>();
    Random rand = new Random();

    private Set<String> userNames = new HashSet<>();
    Map<Integer,String> userStatus = new HashMap<>();

    public int addusername(String name){
        int rand_int1 = rand.nextInt(2);
        int flag = 0;
        String userstat = userStatus.get(userNames.size() - 1);
        // Generate random integers in range 0 to 999

        if(userNames.size() > 0){
            if(userstat.contentEquals("Waiting")){
                flag = 1;
                userStatus.put(userNames.size() - 1, "Ready " + String.valueOf(rand_int1) + " " + gameboard.size());
            }
        }

        userNames.add(name);
        userStatus.put(userNames.size() - 1, "Waiting");
        if(flag == 1){
            String status = "Ready " + String.valueOf(1 - rand_int1) + " " + gameboard.size();
            userStatus.put(userNames.size() - 1, status);
            if(rand_int1 == 1){
                creategame(userNames.size() - 1, userNames.size() - 2, gameboard.size());
            }
            else{
                creategame(userNames.size() - 2, userNames.size() - 1, gameboard.size());
            }
        }
        int val = userNames.size() - 1;
        return val;
    }

    public void updategameboardstatus(int gamenumber, String gameboardstatus){
        gameboard.put(gamenumber, gameboardstatus);
    }

    public void creategame(int user1, int user2, int gameboardsize){
        gameboard.put(gameboardsize, newBoard());
        games.put(user1, "1 " + gameboardsize);
        games.put(user2, "2 " + gameboardsize);

    }

    public String getgamestatus(int usernumber){

        return games.get(usernumber);
    }

    public String getgameboardstatus(int gamenumber){

        return gameboard.get(gamenumber);
    }

    public void restart(int usernumber, int gamenumber){
        String restart = "Restart";
        String playing = "Playing";
        userStatus.put(usernumber, restart);
        Set< Map.Entry<Integer,String> > set = games.entrySet();
        for (Map.Entry<Integer,String> me:set)
        {
            String first_val = me.getValue().split(" ")[1];
            String second_val = me.getValue().split(" ")[0];
            if(first_val.contentEquals(String.valueOf(gamenumber)) && second_val.contentEquals(String.valueOf(1)) && me.getKey() != usernumber){
                int key = me.getKey();
                if(userStatus.get(key).contentEquals(restart)){
                    creategame(key, usernumber, gamenumber);
                    userStatus.put(key, playing);
                    userStatus.put(usernumber, playing);
                }
            }
            else if(first_val.contentEquals(String.valueOf(gamenumber)) && second_val.contentEquals(String.valueOf(2)) && me.getKey() != usernumber){
                int key = me.getKey();
                if(userStatus.get(key).contentEquals(restart)){
                    creategame(usernumber, key, gamenumber);
                    userStatus.put(key, playing);
                    userStatus.put(usernumber, playing);
                }
            }
        }
    }

    public String getstatus(int usernumber){

        return userStatus.get(usernumber);
    }

    private String newBoard(){
        return "123456789_0";
    }

    public void exit(int usernumber, int gamenumber){
        userStatus.put(usernumber, "Left");
        gameboard.put(gamenumber, "Done");
        Set< Map.Entry<Integer,String> > set = games.entrySet();
        for (Map.Entry<Integer,String> me:set)
        {
            if(me.getValue().split(" ")[1].contentEquals(String.valueOf(gamenumber)) && me.getKey() != usernumber){
                userStatus.put(me.getKey(), "Left");
            }
        }
    }
}
