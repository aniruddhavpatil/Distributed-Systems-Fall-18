import com.sun.xml.internal.ws.policy.privateutil.PolicyUtils;
import jdk.internal.util.xml.impl.Pair;

import java.io.*;
import java.lang.reflect.Array;
import java.util.*;
import java.net.*;

public class Server
{

    static Map<ClientHandler, String> subscriptionBook = new HashMap<>();
    static Set<String> userBase = new HashSet<>();

    static int i = 0;
    static int limit;
    static String ip;
    static int port;
    public static void main(String[] args) throws IOException
    {
        System.out.println("Started server with max " + args[0] + " clients");
        limit = Integer.parseInt(args[0]);
        ip = args[1];
        port = Integer.parseInt(args[2]);

        if(args.length < 3 || args.length > 3){
            System.out.println("bash ./client.sh <maxclients> <ip> <port>");
            System.exit(0);
        }

        ServerSocket ss = new ServerSocket(port, 0 , InetAddress.getByName(ip));

        Socket s;


        while (true)
        {

            boolean successfulConnection = false;

                s = ss.accept();

                System.out.println("New client request received : " + s);


                DataInputStream dis = new DataInputStream(s.getInputStream());
                DataOutputStream dos = new DataOutputStream(s.getOutputStream());

                if(i >= limit) {
                    dos.writeUTF("Client limit reached");
                }
                else {

                    String currClientName = "";
                    while (!successfulConnection) {
                        try {
                            currClientName = dis.readUTF();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        if (!currClientName.equals("")) successfulConnection = true;
                    }

                    if(userBase.contains(currClientName)){
                        dos.writeUTF("Username already exists");
                    }
                    else {
                        userBase.add(currClientName);
                        ClientHandler mtch = new ClientHandler(s, currClientName, dis, dos);
                        System.out.println("Created a new handler for new client " + currClientName);
                        Server.subscriptionBook.put(mtch, "");
                        i++;
                        Thread t = new Thread(mtch);

                        System.out.println("Adding this client to active client list");
                        t.start();
                        System.out.println("Number of clients: " + i);
                    }
                }
        }
    }
}

class ClientHandler implements Runnable
{
    Scanner scn = new Scanner(System.in);
    final String name;
    private String currRoom;
    final DataInputStream dis;
    final DataOutputStream dos;

    Socket s;
    boolean isloggedin;

    public ClientHandler(Socket s, String name,
                         DataInputStream dis, DataOutputStream dos) {
        this.dis = dis;
        this.dos = dos;
        this.name = name;
        this.s = s;
        this.isloggedin=true;
        this.currRoom = "";
    }

    public void sendMessage(String message){
        try{
            dos.writeUTF(message);
        } catch (IOException e){
            e.printStackTrace();
        }
    }

    public void sendError(String errorMessage){
        try{
            dos.writeUTF("ERROR: " + errorMessage);
        } catch (IOException e){
            e.printStackTrace();
        }
    }

    public void leaveRoom(){

        for (Map.Entry<ClientHandler, String> broadcaster : Server.subscriptionBook.entrySet()) {
            if (broadcaster.getValue().equals(this.currRoom))
                broadcaster.getKey().sendMessage(this.name + " has left " + this.currRoom);
        }
        Server.subscriptionBook.put(this, "");
        this.currRoom = "";
    }

    @Override
    public void run() {

        String received;
        while (true)
        {
            try
            {
                received = dis.readUTF();

                System.out.println(received);

                if(received.trim().isEmpty()){
                    this.sendError("No command received");
                }
                else {
                    StringTokenizer st = new StringTokenizer(received);
                    String command = st.nextToken();
                    String command_type = "";
                    String argument = "";

                    if (!command.equals("reply") && st.countTokens() > 3) sendError("Too many arguments");
                    else {
                        switch (command) {
                            case "create":
                                command_type = st.nextToken();
                                if (command_type.equals("chatroom")) {
                                    argument = st.nextToken();
                                    if (!Server.subscriptionBook.containsValue(argument)) {
                                        if (!this.currRoom.equals("")) this.leaveRoom();
                                        Server.subscriptionBook.put(this, argument);
                                        System.out.println("Created room: " + argument + " and added user: " + this.name);
                                        this.currRoom = argument;
                                    } else this.sendError("Room already exists.");
                                } else sendError("Unrecognized command instruction");
                                break;

                            case "list":
                                command_type = st.nextToken();
                                if (command_type.equals("chatrooms")) {
                                    Set<String> S = new HashSet<>(Server.subscriptionBook.values());
                                    System.out.println(S.removeAll(Arrays.asList("", null)));
                                    try {
                                        dos.writeUTF(S.toString());
                                    } catch (IOException e) {
                                        e.printStackTrace();
                                    }
                                } else sendError("Unrecognized command instruction");
                                break;

                            case "join":
                                if(!st.hasMoreElements()) sendError("Too few arguments");
                                else {
                                    argument = st.nextToken();
                                    if (Server.subscriptionBook.containsValue(argument)) {
                                        if (!this.currRoom.equals("")) this.leaveRoom();
                                        Server.subscriptionBook.put(this, argument);
                                        this.currRoom = argument;
                                        for (Map.Entry<ClientHandler, String> entry : Server.subscriptionBook.entrySet()) {
                                            if (entry.getValue().equals(this.currRoom))
                                                entry.getKey().sendMessage(this.name + " has joined " + this.currRoom);
                                        }
                                    } else this.sendError("Room does not exist. Staying in the same room.");
                                }
                                break;
                            case "leave":
                                if (this.currRoom.equals(""))
                                    this.sendError("Not a part of any room. Leaving room failed.");
                                else this.leaveRoom();
                                break;

                            case "add":
                                argument = st.nextToken();
                                if (this.currRoom.equals(""))
                                    this.sendError("Not a part of any room. Adding user failed.");
                                else {
                                    boolean foundUser = false;
                                    for (Map.Entry<ClientHandler, String> entry : Server.subscriptionBook.entrySet()) {
                                        ClientHandler temp = entry.getKey();
                                        if (temp.name.equals(argument)) {
                                            foundUser = true;
                                            if (temp.currRoom.equals("")) {
                                                Server.subscriptionBook.put(temp, this.currRoom);
                                                temp.sendMessage(this.name + " added you to the room " + this.currRoom);
                                                temp.currRoom = this.currRoom;
                                                for (Map.Entry<ClientHandler, String> broadcaster : Server.subscriptionBook.entrySet()) {
                                                    if (broadcaster.getValue().equals(this.currRoom))
                                                        broadcaster.getKey().sendMessage(this.name + " has added " + temp.name + " to this room: " + this.currRoom);
                                                }
                                            } else
                                                this.sendError("User " + temp.name + " is already a part of a room.");
                                        }
                                    }
                                    if (!foundUser) this.sendError("User " + argument + " not found.");
                                }
                                break;
                            case "whoami":
                                sendMessage("You are logged in as: " + this.name);
                                break;

                            case "reply":
                                if(!st.hasMoreElements()) sendError("Too few arguments");
                                else if (this.currRoom.equals("")) sendError("Not associated with a chatroom.");
                                else {
                                    for (Map.Entry<ClientHandler, String> entry : Server.subscriptionBook.entrySet()) {
                                        if (entry.getValue().equals(this.currRoom))
                                            entry.getKey().sendMessage(this.name + " : " + received.substring(5));
                                    }
                                }
                                break;

                            case "send":
                                if(!st.hasMoreElements()) sendError("Too few arguments");
                                else{
                                    String fileName = st.nextToken();
                                    String protocol = st.nextToken();
                                    Socket socket = new Socket("localhost", 9000);

                                    if(protocol.equals("tcp")) {

                                        BufferedInputStream in =
                                                new BufferedInputStream(socket.getInputStream());

                                        BufferedOutputStream out =
                                                new BufferedOutputStream(new FileOutputStream("./serverFiles/" + fileName));
                                        System.out.println("Receiving file from client");
                                        int len = 0;
                                        byte[] buffer = new byte[8192];
                                        while ((len = in.read(buffer)) > 0) {
                                            out.write(buffer, 0, len);

                                        }

                                        in.close();
                                        out.flush();
                                        out.close();
                                        socket.close();
                                        System.out.println("Received file from client");


                                        for (Map.Entry<ClientHandler, String> entry : Server.subscriptionBook.entrySet()) {

                                            if (entry.getValue().equals(this.currRoom) && entry.getKey() != this) {

                                                ClientHandler curr = entry.getKey();
                                                System.out.println("Sending " + fileName + " to " + curr.name);
                                                curr.sendMessage("Receiving " + fileName + " from " + this.name);
                                                curr.sendMessage("FT:TCP " + fileName);

                                                ServerSocket sendSocket = new ServerSocket(9001);
                                                Socket client = sendSocket.accept();
                                                in =
                                                        new BufferedInputStream(
                                                                new FileInputStream("./serverFiles/" + fileName));

                                                out =
                                                        new BufferedOutputStream(client.getOutputStream());
                                                len = 0;
                                                buffer = new byte[8192];
                                                while ((len = in.read(buffer)) > 0) {
                                                    out.write(buffer, 0, len);
                                                }
                                                in.close();
                                                out.flush();
                                                out.close();
                                                client.close();
                                                sendSocket.close();
                                                curr.sendMessage("Received " + fileName + " from " + this.name);
                                            }
                                        }
                                    }
                                    else{

                                        BufferedInputStream in =
                                                new BufferedInputStream(socket.getInputStream());

                                        BufferedOutputStream out =
                                                new BufferedOutputStream(new FileOutputStream("./serverFiles/" + fileName));
                                        System.out.println("Receiving file from client");
                                        int len = 0;
                                        byte[] buffer = new byte[8192];
                                        while ((len = in.read(buffer)) > 0) {
                                            out.write(buffer, 0, len);

                                        }

                                        in.close();
                                        out.flush();
                                        out.close();
                                        socket.close();
                                        System.out.println("Received file from client");


                                        for (Map.Entry<ClientHandler, String> entry : Server.subscriptionBook.entrySet()) {

                                            if (entry.getValue().equals(this.currRoom) && entry.getKey() != this) {

                                                ClientHandler curr = entry.getKey();
                                                System.out.println("Sending " + fileName + " to " + curr.name);
                                                curr.sendMessage("Receiving " + fileName + " from " + this.name);
                                                curr.sendMessage("FT:UDP " + fileName);

                                                ServerSocket sendSocket = new ServerSocket(9001);
                                                Socket client = sendSocket.accept();
                                                in =
                                                        new BufferedInputStream(
                                                                new FileInputStream("./serverFiles/" + fileName));

                                                out =
                                                        new BufferedOutputStream(client.getOutputStream());
                                                len = 0;
                                                buffer = new byte[8192];
                                                while ((len = in.read(buffer)) > 0) {
                                                    out.write(buffer, 0, len);
                                                }
                                                in.close();
                                                out.flush();
                                                out.close();
                                                client.close();
                                                sendSocket.close();
                                                curr.sendMessage("Received " + fileName + " from " + this.name);
                                            }
                                        }

                                    }
                                }
                                break;

                            default:
                                System.out.println("Did not recognize command: " + command);
                                this.sendError("Did not recognize command: " + command);
                                break;

                        }
                    }
                }

            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    }
}