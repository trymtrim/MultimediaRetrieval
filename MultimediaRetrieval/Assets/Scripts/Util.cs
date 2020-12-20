using System.IO;

public static class Util
{
    public static string GetDatabasePath()
    {
        return Path.GetFullPath("Backend/database.db");
    }
}