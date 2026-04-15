//! Сериализация ростера: owner|uid:role;uid2:role2;...
class TFA_Roster
{
	//! Только участники (без префикса владельца), для RPC в GUI.
	static string SerializeMembersOnly(map<string, int> members, string excludeOwnerUid = "")
	{
		string s = "";
		for (int i = 0; i < members.Count(); i++)
		{
			string uid = members.GetKey(i);
			int role = members.GetElement(i);
			if (uid == "")
				continue;
			if (excludeOwnerUid != "" && uid == excludeOwnerUid)
				continue;
			s = s + uid + ":" + role.ToString() + ";";
		}
		return s;
	}

	static string Serialize(string ownerUid, map<string, int> members)
	{
		string s = ownerUid + "|";
		for (int i = 0; i < members.Count(); i++)
		{
			string uid = members.GetKey(i);
			int role = members.GetElement(i);
			if (uid != "" && uid != ownerUid)
				s = s + uid + ":" + role.ToString() + ";";
		}
		return s;
	}

	static void DeserializeMembers(string blob, map<string, int> members)
	{
		if (!members)
			return;
		members.Clear();
		if (!blob || blob == "")
			return;
		TStringArray parts = new TStringArray;
		blob.Split(";", parts);
		for (int i = 0; i < parts.Count(); i++)
		{
			string pair = parts[i];
			if (pair == "")
				continue;
			int c = pair.IndexOf(":");
			if (c < 1)
				continue;
			string uid = pair.Substring(0, c);
			string rs = pair.Substring(c + 1, pair.Length() - c - 1);
			members.Set(uid, rs.ToInt());
		}
	}

	static void Deserialize(string data, out string ownerUid, out map<string, int> members)
	{
		ownerUid = "";
		members = new map<string, int>;
		if (!data || data == "")
			return;

		int sep = data.IndexOf("|");
		if (sep < 0)
		{
			ownerUid = data;
			return;
		}

		ownerUid = data.Substring(0, sep);
		string rest = data.Substring(sep + 1, data.Length() - sep - 1);
		TStringArray parts = new TStringArray;
		rest.Split(";", parts);

		for (int i = 0; i < parts.Count(); i++)
		{
			string pair = parts[i];
			if (pair == "")
				continue;
			int c = pair.IndexOf(":");
			if (c < 1)
				continue;
			string uid = pair.Substring(0, c);
			string rs = pair.Substring(c + 1, pair.Length() - c - 1);
			int role = rs.ToInt();
			if (uid != "" && uid != ownerUid)
				members.Set(uid, role);
		}
	}
}
