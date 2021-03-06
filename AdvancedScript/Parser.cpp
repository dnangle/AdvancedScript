#pragma once
#include "Parser.h"
#include "ScriptFun.h"
#include "LogTemplate.h"
#include "adsLib.h"
//////////////////////////////////////////////
//notes:
//- all numbers are decimal
//- all hex numbers are begin with 0x  x small latter

//////////////////////////////////////////////

int GetRegisterIndex(String^ input_) {
	array <String^>^ RegisterEnum_ = { "DR0" ,"DR1" ,"DR2" ,"DR3" ,"DR6" ,
		"DR7" ,"EAX" ,"AX" ,"AH" ,"AL" ,"EBX" ,"BX" ,"BH" ,"BL" ,"ECX" ,
		"CX" ,"CH" ,"CL" ,"EDX" ,"DX" ,"DH" ,"DL" ,"EDI" ,"DI" ,"ESI" ,
		"SI" ,"EBP" ,"BP" ,"ESP" ,"SP" ,"EIP" ,"RAX" ,"RBX" ,"RCX" ,"RDX" ,
		"RSI" ,"SIL" ,"RDI" ,"DIL" ,"RBP" ,"BPL" ,"RSP" ,"SPL" ,"RIP" ,
		"R8" ,"R8D" ,"R8W" ,"R8B" ,"R9" ,"R9D" ,"R9W" ,"R9B" ,"	R10" ,"R10D" ,
		"R10W" ,"R10B" ,"R11" ,"R11D" ,"R11W" ,"R11B" ,"R12" ,"R12D" ,"R12W" ,
		"R12B" ,"R13" ,"R13D" ,"R13W" ,"R13B" ,"R14" ,"R14D" ,"R14W" ,"R14B" ,
		"R15" ,"R15D" ,"R15W" ,"R15B" ,"CIP" ,"CSP" ,"CAX" ,"CBX" ,"CCX" ,"CDX" ,
		"CDI" ,"CSI" ,"CBP" ,"	CFLAGS" };

	return RegisterEnum_->IndexOf(RegisterEnum_, input_); // if -1 then not found 
}

String^ readVarName(String^ input, int arrayIndex, String^% VarString2Replace) {
	String^ temp = input;
	String^ value_ = "";
	String^ value_1 = "";
	String^ vartype = "";
	int arrayLength;
	int index_ = 0;
	int index_t = 0;
	for (int i = 0; i < temp->Length; i++)
	{
		value_ = value_ + temp->Substring(i, 1);
		if (i + 1 == temp->Length) {	/// if this later is the end of string 
			if (Varexist(value_, vartype, index_, arrayLength)) {
				VarString2Replace = value_;
				return ScriptFunList::VarList[index_]->varvalue[arrayIndex];
			}
			else { return "NULL/ "; }
		}
		if (temp->Substring(i + 1, 1) == " ") { /// if the next letter is space
			if (Varexist(value_, vartype, index_, arrayLength)) {
				VarString2Replace = value_;
				return ScriptFunList::VarList[index_]->varvalue[arrayIndex];
			}
		}
		while (Varexist(value_, vartype, index_, arrayLength))
		{
			if (i + 1 < temp->Length) {
				i += 1;
				value_ = value_ + temp->Substring(i, 1);
			}
			else
			{
				VarString2Replace = value_;
				return ScriptFunList::VarList[index_]->varvalue[arrayIndex];
				//break;
			}

		}

	}
	return "NULL/ ";
}

String^ findVarValue(String^ input, VarType retAsVartype, String^% VarString) {
	// find the variable begin with $ like $x or $x[1]  // this VarString will used for replacing string
	// In case Var is int then no need to check for Hex validation 
	// but if the Var is string we need to check for hex validation and shift it to int 	
	String^ ArrayIndexValue = "";
	String^ var_name = ""; int arrayLength;
	String^ vartype_ = ""; int indexofVar = 0;
	if (!input->Contains("$")) {
		return "NULLx/ no Variable name";  // this mean there are no variable in this string
	}
	else {
		VarString = "$";
		input = input->Substring(1, input->Length - 1);  // we reomved $ from the begining 
	}
	if ((input->IndexOf("[") > 0) && (input->IndexOf("$") < 0)) {  // variable is Array  /// we must check if there are another var ( contain $ ) so we not mix with other var's		
		var_name = input->Substring(0, input->IndexOf("["));  // get Var name			
		if (Varexist(var_name->Trim(), vartype_, indexofVar, arrayLength)) { /// check if var exist  // we clear space here just , because we need to build VarString
			if (vartype_ == "array" && input->IndexOf("]") > 0) { // var type must be array and the rest of string must have close ]
				for (int i = input->IndexOf("[") + 1; i < input->Length; i++) //get index of var
				{
					if (input->Substring(i, 1) != "]") {
						ArrayIndexValue = ArrayIndexValue + input->Substring(i, 1);
					}
					else {
						VarString = VarString + input; // this is the end index of var we will used later to replace string
						break;
					}
				}
				ArrayIndexValue = GetArgValueByType(ArrayIndexValue, VarType::int_); /// now check if the index, maybe Numeric or variable
				if (ArrayIndexValue->StartsWith("NULL/")) {
					return "NULL/ something go wrong in resolve index";
				}
				if (Information::IsNumeric(ArrayIndexValue)) {  /// if it's resolved as int value then we used it 
					if ((int)Str2duint(ArrayIndexValue) > ScriptFunList::VarList[indexofVar]->arrayLength - 1) { // -1 because array begin from 0
						return "NULL/ value beggier than array index";
					}
					if (retAsVartype == VarType::str) {/// if we need ret var value as string it will back as str no need to change the value
						return ScriptFunList::VarList[indexofVar]->varvalue[Str2duint(ArrayIndexValue)];
					}
					else //// but if the retAsVartype (ret value of the var) is int so we need to resolve it as int because array hold string
					{
						String^ intValue;  /// we ret the int value from the array item
						if (ScriptFunList::VarList[indexofVar]->varvalue[Str2duint(ArrayIndexValue)] == nullptr) {
							return "NULL/ no value for this cell in the array";
						}
						if (CheckHexIsValid(ScriptFunList::VarList[indexofVar]->varvalue[Str2duint(ArrayIndexValue)], intValue) == 0) {  /// /// check the value of the vra if it's not Numeric then if could be Hex
							return "NULL/ array value is not Numeric";/// that something wrong in the index of the array 
						}
						else {
							return intValue;
						}
					}
				}
				String^ intValue;
				if ((int)Str2duint(ArrayIndexValue) > ScriptFunList::VarList[indexofVar]->arrayLength - 1) { // -1 because array begin from 0
					return "NULL/ value beggier than array index";
				}
				if (CheckHexIsValid(ArrayIndexValue, intValue) == 0) {  /// /// check if array index is not Numeric then if could be Hex
					return "NULL/ array index is not Numeric";/// that something wrong in the index of the array 
				}
				else {
					if (retAsVartype == VarType::str) {/// if we need ret var value as string it will back as str no need to change the value
						return ScriptFunList::VarList[indexofVar]->varvalue[Str2duint(intValue)];
					}
					else //// but if the ret value of the var is int so we need to resolve it as int
					{
						String^ intValue1;  /// we ret the int value from the array 
						if (CheckHexIsValid(ScriptFunList::VarList[indexofVar]->varvalue[Str2duint(intValue)], intValue1) == 0) {  /// /// check the value of the vra if it's not Numeric then if could be Hex
							return "NULL/ array value is not Numeric";/// that something wrong in the index of the array 
						}
						else {
							return intValue1; // return the value of the array var
						}
					}
				}
			}
			else
			{
				return "NULL/ Variable not as an array in the list, or u missing ]";/// that something wrong in the index of the array 
			}
		}
		else {
			return "NULL/ Variable not in the list";/// that something wrong in the index of the array 
		}
	}
	if (input->IndexOf(" ") > 0) {  /// var is int or str		
		var_name = input->Substring(0, input->IndexOf(" "));  // get Var name	and clear spaces	
		if (Varexist(var_name->Trim(), vartype_, indexofVar, arrayLength)) { /// check if var exist	
			VarString = "$" + var_name; // this is the end index of var we will used later to replace string

			if (retAsVartype == VarType::str) {/// if we need ret var value as string it will back as str no need to change the value
				//return ScriptFunList::VarList[indexofVar]->varvalue[0];
				if (vartype_ == "int") {  /// if retAsVartype is string and vartype_ is int then we convert it to hex value
					return  duint2Hex(Str2duint(ScriptFunList::VarList[indexofVar]->varvalue[0])); // return the value of the int var as hex
				}
				else { /// case str 
					return  ScriptFunList::VarList[indexofVar]->varvalue[0]; // return var string as it
				}
			}
			else //// but if the retAsVartype( ret value of the var) is int so we need to resolve it as int
			{
				/// now we need to check the var type to ret the value of int and checkValue of the str
				if (vartype_ == "int") {  /// in case it is int no need to check Hex 
					return ScriptFunList::VarList[indexofVar]->varvalue[0]; // return the value of the int var as it
				}
				else {  /// in case it str we need to check for hex validation and convert to int
					String^ varvalue = StrAnalyze(ScriptFunList::VarList[indexofVar]->varvalue[0], VarType::int_);
					//String^ intValue;
					//if (CheckHexIsValid(varvalue, intValue) == 0) {
					if (!Information::IsNumeric(varvalue)) {
						return "NULL/ Value of str var is not Numeric";
					}
					else {
						return varvalue; // return the value 
					}
				}
			}
		}
	}
	else
	{  /// there are no space at the end we need to search the rest of the string	  
		var_name = input;   /// we didn't find it as array or didn't find space then we get all string as var name
		if (Varexist(var_name->Trim(), vartype_, indexofVar, arrayLength)) { /// check if var exist	
			VarString = "$" + var_name; // this is the end index of var we will used later to replace string

			if (retAsVartype == VarType::str) {/// if we need ret var value as string it will back as str no need to change the value				
				//return ScriptFunList::VarList[indexofVar]->varvalue[0];
				if (vartype_ == "int") {  /// if retAsVartype is string and vartype_ is int then we convert it to hex value
					return  duint2Hex(Str2duint(ScriptFunList::VarList[indexofVar]->varvalue[0])); // return the value of the int var as it
				}
				else { /// case str 
					return  ScriptFunList::VarList[indexofVar]->varvalue[0]; // return var string as it
				}
			}
			else //// but if the retAsVartype( ret value of the var) is int so we need to resolve it as int
			{
				/// now we need to check the var type to ret the value of int and checkValue of the str
				if (vartype_ == "int") {  /// in case it is int no need to check Hex 
					return ScriptFunList::VarList[indexofVar]->varvalue[0]; // return the value of the int var as it
				}
				else {  /// in case it str we need to check for hex validation and convert to int
					// we need to resolve the value to check if can get int from it 
					String^ varvalue = StrAnalyze(ScriptFunList::VarList[indexofVar]->varvalue[0], VarType::int_);
					//String^ intValue;
					//if (CheckHexIsValid(varvalue, intValue) == 0) {
					if (!Information::IsNumeric(varvalue)) {
						return "NULL/ Value of str var is not Numeric";
					}
					else {
						return varvalue; // return the value 
					}
				}

			}
		}
	}
	return "NULL/ no Variable name";  // this mean there are no variable in this string 
}

String^ findScriptSystemVarValue(String^ input) {
	if (input->IndexOf("}") > 0) {
		//String^ VarString = input->Substring(input->IndexOf("{", input->Length - input->IndexOf("}")));
		return StringFormatInline_Str(input);
	}
	else
	{
		return "NULL/ Missing } at the end";
	}


}

String^ ForWard(String^ input, int tokenindex, String^% VarString) { /// tokenindex is hold the index of token
	String^ temp = input;
	String^ value_ = "";
	array <String^>^ tokens_ = { "*" ,"/" ,"+" ,"-" ,"$" ," " ,"(",")" };
	tokenindex = tokenindex + 1; // exclude token form the string
	if (tokenindex + 1 > temp->Length)
		return "NULL/ ";  /// token at the end of string
	else
	{
		temp = temp->Substring(tokenindex, temp->Length - tokenindex); // get value after token
		int i1 = 0;
		if (i1 + 1 < temp->Length) {
			while (temp->Substring(i1 + 1, 1) == " ")  /// count how many spaces
			{
				VarString = " " + VarString;
				if (i1 + 1 > input->Length) { break; } // if we reach the begin of the string
				i1 += 1;
			}
		}
		temp = temp->Trim(); /// remove all spaces after token like >>  55   + 10
		int i = 0;
		while (Array::IndexOf(tokens_, temp->Substring(i, 1)) < 0) /// should not be in this list
		{
			value_ = value_ + temp->Substring(i, 1);
			i += 1;
			if (i + 1 > temp->Length) { break; } // if we reach the begin of the string
		}
		if (value_ == "") {
			value_ = temp;
		}

		VarString = value_ + VarString;
		if (Information::IsNumeric(value_)) {   /// check it if it's number value 
			return value_;
		}
		else {  /// that mean it hold variable or something else			
			return GetArgValueByType(value_, VarType::int_);
		}
	}
	return "NULL/ ";
}

String^ BackWard(String^ input, int tokenindex, String^% VarString) {
	String^ temp = input;
	String^ value_ = "";
	array <String^>^ tokens_ = { "*" ,"/" ,"+" ,"-" ,"$" ," " ,"(",")" };
	if (tokenindex - 1 < 0)
		return "NULL/ ";  /// token at the begin of string
	else
	{
		temp = temp->Substring(0, tokenindex); // get value before token
		int i1 = temp->Length;
		while (temp->Substring(i1 - 1, 1) == " ")  /// count how many spaces
		{
			VarString = VarString + " ";
			if (i1 - 1 < 0) { break; } // if we reach the begin of the string
			i1 -= 1;
		}
		temp = temp->Trim(); /// remove all spaces
		int i = temp->Length;
		while (Array::IndexOf(tokens_, temp->Substring(i - 1, 1)) < 0) /// should not be in this list
		{
			value_ = temp->Substring(i - 1, 1) + value_;
			i -= 1;
			if (i - 1 < 0) { break; } // if we reach the begin of the string
		}
		if (value_ == "") {
			value_ = temp;
		}
		VarString = value_ + VarString;
		if (Information::IsNumeric(value_)) {   /// check it if it's hex(number) value 
			return value_;
		}
		else {  /// that mean it hold variable or something else			
			return GetArgValueByType(value_, VarType::int_);
		}
	}
	return "NULL/ ";
}

String^ tokens(String^ input, String^% VarString) {
	// check for * first 
	String^ VarString1 = "";
	String^ VarString2 = "";
	String^ para1 = ""; 	String^ para2 = "";
	if (input->StartsWith("-")) {   /// in case we have negtive numbers
		String^ IntValue;
		if (CheckHexIsValid(input->Substring(1, input->Length - 1), IntValue) > 0) {
			return "-" + int2Str(Hex2duint(input->Substring(1, input->Length - 1)));  /// we return the value as int add (-) to it
		}
		else {
			return input;
		}

	}
	if (input->IndexOf("*") > 0) { /// should be bigger than 0 , token should not be at the begining of the exprsion 		
		para1 = BackWard(input, input->IndexOf("*"), VarString1);
		para2 = ForWard(input, input->IndexOf("*"), VarString2);  // we begin after token		
		VarString = VarString1 + "*" + VarString2;
		if ((!Information::IsNumeric(para1)) || !Information::IsNumeric(para2))
			return "NULL/ ";
		else {
			return Conversion::Str(Conversion::Val(para1) * Conversion::Val(para2));
		}
	}

	if (input->IndexOf("/") > 0) { /// should be bigger than 0 , token should not be at the begining of the exprsion 		
		para1 = BackWard(input, input->IndexOf("/"), VarString1);
		para2 = ForWard(input, input->IndexOf("/"), VarString2);
		VarString = VarString1 + "/" + VarString2;
		if ((!Information::IsNumeric(para1)) || !Information::IsNumeric(para2))
			return "NULL/ ";
		else {
			return Conversion::Str(Conversion::Val(para1) / Conversion::Val(para2));
		}
	}

	if (input->IndexOf("+") > 0) { /// should be bigger than 0 , token should not be at the begining of the exprsion 		
		para1 = BackWard(input, input->IndexOf("+"), VarString1);
		para2 = ForWard(input, input->IndexOf("+"), VarString2);
		VarString = VarString1 + "+" + VarString2;
		if ((!Information::IsNumeric(para1)) || !Information::IsNumeric(para2)) {
			_plugin_logprint("one of this arguments are not numeric");
			return "NULL/ ";
		}
		else {
			return Conversion::Str(Conversion::Val(para1) + Conversion::Val(para2));
		}
	}

	if (input->IndexOf("-") > 0) { /// should be bigger than 0 , token should not be at the begining of the exprsion 		
		para1 = BackWard(input, input->IndexOf("-"), VarString1);
		para2 = ForWard(input, input->IndexOf("-"), VarString2);
		VarString = VarString1 + "-" + VarString2;
		if ((!Information::IsNumeric(para1)) || !Information::IsNumeric(para2))
			return "NULL/ ";
		else {
			return Conversion::Str(Conversion::Val(para1) - Conversion::Val(para2));
		}
	}
	return "NULL/ ";
}

String^ findHexValue(String^ input, String^% oldvalue_) {
	String^ temp;
	if (input->IndexOf("0x") >= 0) {
		input = input->Substring(input->IndexOf("0x") + 2, input->Length - (input->IndexOf("0x") + 2));
		oldvalue_ = "0x";
	}
	else
	{
		return "NULL/ ";
	}
	for (int i = 0; i < input->Length; i++)
	{
		String^ intValue1; String^ intValue2;
		if (CheckHexIsValid(input->Substring(i, 1), intValue1)  > 0) {  // if the char not hex or numeric 
			temp = temp + input->Substring(i, 1);
			if (i + 1 < input->Length) {
				if (CheckHexIsValid(input->Substring(i + 1, 1), intValue2) == 0) {
					oldvalue_ = oldvalue_ + temp;
					break;
					//return  int2Str(Hex2duint(temp));    /// now we return the value as int stored in str var
				}
			}
		}
	}
	return  int2Str(Hex2duint(temp));    /// now we return the value as int stored in str var
}

String^ resolveString(String^ input, int% commaCount) {   //// used to resolve strings in str or array vriables 
	String^ temp = "";
	if (input->StartsWith("\"") && (input->EndsWith("\""))) {  /// that mean all string is commaed 
		commaCount = 2;
		return input->Substring(1, input->LastIndexOf("\"") - 1);
	}
	for (int i = 0; i < input->Length; i++)
	{
		if (input->Substring(i, 1) == "\"") {
			commaCount += 1;
		}
	}
	if (commaCount % 2 != 0) {  /// check if there is an open comma for each comma
		commaCount = -1;
		return input;
	}
	if (commaCount == 0) {  /// in case there are no comma
		String^ OldValue_;
		return argumentValue(input, OldValue_);

	}
	for (int i = 0; i < input->Length; i++)
	{
		if (input->Substring(i, 1) != "\"") {
			temp = temp + input->Substring(i, 1);
		}
		if (i + 1 < input->Length) {
			if (input->Substring(i + 1, 1) == "\"") {
				String^ OldValue_;
				temp = argumentValue(temp->Trim(), OldValue_);  /// reolve this str in case it return NULL we take the old value (which could be half resolved)
				if (temp->StartsWith("NULL/ ")) {
					temp = OldValue_;
				}
				int NextCommaIndex = 1 + input->IndexOf("\"", i + 1);
				String^ StringafterComma = input->Substring(NextCommaIndex, input->Length - NextCommaIndex);
				temp = temp + StringafterComma;
				temp = temp->Substring(0, temp->Length - 1);  /// remove the next comma
				i = i + NextCommaIndex + StringafterComma->Length;  /// now jmp to the next comma
			}
		}
	}
	return temp;
}


bool CheckexcutedCmd(String^ cmd_) {
	Generic::List<String^>^ arguments;
	String^ OldValue_;

	if (cmd_->StartsWith("memdump(") || cmd_->StartsWith("memdump (")) {
		GetArg(cmd_->Substring(cmd_->IndexOf("("), cmd_->Length - cmd_->IndexOf("(")), arguments, true);
		String^ addr = StrAnalyze(arguments[0], VarType::str, true);
		String^ Size_ = StrAnalyze(arguments[1], VarType::str);
		if ((addr->StartsWith("NULL/ ")) || (Size_->StartsWith("NULL/ "))) {
			_plugin_logprint("wrong arguments for memdump command");
			return false;
		}
		switch (arguments->Count)
		{
		case 2: {
			dumpmem(addr, Size_);
			return true;
		}
		case 3: {
			dumpmem(addr, Size_, arguments[2]);
			return true;
		}
		default:
			_plugin_logprint("wrong arguments for memdump command");
			return false;
		}
	}
	_plugin_logprint(Str2ConstChar(argumentValue(cmd_, OldValue_)));
	return false;
}


String^ argumentValue(String^ argument, String^% OldValue_) {  /// return the <<int>> if possible ,value of the argument storing in string var
	OldValue_ = argument; // rest the value	
	int isNumOrHex = 0; String^ intValue;
	if (CheckHexIsValid(argument, intValue) > 0) {  // <<< need to know if it's whole str is number or hex 	
		//switch (isNumOrHex)
		//{
		//case 1: {
		//argument = duint2Hex(Str2Int(intValue)); /// if the value is number we get the int value form the hex value
		argument = intValue; /// if the value is number we get the int value form the hex value
		return argument;
		//}
		//case 2: {
		//	argument = int2Str(Hex2duint(argument));// it mean it's already hex format (000045FAB) and we convert it to int we store it in str var
		//	return argument;
		//}
		//default:
		//	break;
		//}
	}
	//////////////////////////////////////
	int commaCount = 0;
	//argument = resolveString(argument, commaCount);
	if (argument->StartsWith("\"") && (argument->EndsWith("\""))) {  /// that mean all string is commaed 
		return argument;
	}
	//////////////////////////////////////
	if (argument->IndexOf("0x") >= 0) {  /// check if we have a hex value in the string
		while (argument->IndexOf("0x") >= 0) {
			String^ replaceValue = "";
			String^ oldvalue;
			String^ tempinput = argument->Substring(argument->IndexOf("0x"), argument->Length - argument->IndexOf("0x"));  // we take the part which have hex value
			replaceValue = findHexValue(tempinput, oldvalue);	  /// return as int		
			OldValue_ = oldvalue;
			if (!replaceValue->StartsWith("NULL/")) {
				argument = ReplaceAtIndex(argument, oldvalue, replaceValue);
			}
			else
				return "NULL/ ";
		}
	}

	if ((argument->IndexOf("{") >= 0) && (argument->IndexOf("}", argument->IndexOf("{")) >= 0)) {
		while (argument->IndexOf("{") >= 0) {
			String^ replaceValue = "";
			String^ oldvalue = argument->Substring(argument->IndexOf("{"), argument->IndexOf("}") + 1);
			replaceValue = findScriptSystemVarValue(oldvalue);
			String^ inValue;
			if (CheckHexIsValid(replaceValue, inValue) > 0) {
				replaceValue = inValue;
			}
			else {
				replaceValue = "NULL/";
			}

			/*int hexOrInt = CheckHexIsValid(replaceValue, inValue);
			if (hexOrInt < 0) {    /// in case the value less than 0 so no need to get the hex we just get the entered first time
				replaceValue = "NULL/";
			}
			if (hexOrInt == 1) {
				replaceValue = inValue;
			}
			if (hexOrInt == 2) {
				replaceValue = replaceValue;
			}*/
			OldValue_ = oldvalue;
			if (!replaceValue->StartsWith("NULL/")) {
				argument = ReplaceAtIndex(argument, oldvalue, replaceValue);
			}
			else
			{
				return "NULL/ ";
			}

		}
	}
	/// find Variables Local System from VarList ,all variable should have $ at the begining like $x or $x[1]
	if (argument->Contains("$")) {
		String^ tempInput = argument;
		while (argument->IndexOf("$") >= 0)
		{
			String^ oldValue = "";
			tempInput = argument->Substring(argument->IndexOf("$"), argument->Length - argument->IndexOf("$"));
			tempInput = findVarValue(tempInput, VarType::int_, oldValue);
			int isNumOrHex1 = 0; String^ intValue1;
			if (CheckHexIsValid(tempInput, intValue1) > 0) {  // <<< need to know if it's number or hex 
				tempInput = intValue1;
			}
			OldValue_ = oldValue;
			if (tempInput->StartsWith("NULL/")) {
				_plugin_logprint(Str2ConstChar(tempInput));
				return "NULL/ ";
			}
			else {
				//argument = argument->Replace (VarString, tempInput);
				argument = ReplaceAtIndex(argument, oldValue, tempInput);
			}
		}
	}


	if ((argument->Contains("*")) || (argument->Contains("+")) || (argument->Contains("-")) || (argument->Contains("/"))) {  /// I will do it later
		while ((argument->Contains("*")) || (argument->Contains("+")) || (argument->Contains("-")) || (argument->Contains("/")))
		{
			String^ tempInput = argument;
			String^ oldValue = "";
			tempInput = tokens(tempInput, oldValue);  /// we will get them as int value			
			if (tempInput->StartsWith("NULL/")) {
				OldValue_ = argument;
				_plugin_logprint(Str2ConstChar(tempInput));
				return "NULL/ ";
			}
			else {
				argument = ReplaceAtIndex(argument, oldValue, tempInput);
			}
			if (Information::IsNumeric(argument)) {  /// in case it back negtive value
				if (Str2duint(argument) < 0) {
					break;
				}
			}
		}
	}

	return argument;
}

String^ GetArgValueByType(String^ argument, VarType type_, bool Add0x) {  /// return value by type	

	switch (type_)
	{
	case int_:
	{
		String^ intValue;
		if ((argument->Contains("\"")) || (argument->Contains("\\"))) {  /// int value should not contain " or "\\"
			return "NULL/ ";
		}
		if (CheckHexIsValid(argument, intValue) > 0) {
			argument = (intValue)->Trim(); /// if the value is number we get the int value form the hex value
			return argument;
		}
		if (argument->IndexOf("0x") >= 0) {  /// check if we have a hex value in the string
			while (argument->IndexOf("0x") >= 0) {
				String^ replaceValue = "";
				String^ oldvalue;
				String^ tempinput = argument->Substring(argument->IndexOf("0x"), argument->Length - argument->IndexOf("0x"));  // we take the part which have hex value
				replaceValue = findHexValue(tempinput, oldvalue);	  /// return as int		
				if (!replaceValue->StartsWith("NULL/")) {
					argument = (ReplaceAtIndex(argument, oldvalue, replaceValue))->Trim();
				}
				else { return "NULL/ "; }
			}
		}
		if ((argument->IndexOf("{") >= 0) && (argument->IndexOf("}", argument->IndexOf("{")) >= 0)) {
			while (argument->IndexOf("{") >= 0) {
				String^ replaceValue = "";
				String^ oldvalue = argument->Substring(argument->IndexOf("{"), argument->IndexOf("}") + 1);
				replaceValue = findScriptSystemVarValue(oldvalue);
				String^ inValue;  /// now the value should be int other wise wrong value to handle
				if (CheckHexIsValid(replaceValue, inValue) > 0) {
					replaceValue = inValue;
				}
				if (!inValue->StartsWith("NULL/")) {
					argument = (ReplaceAtIndex(argument, oldvalue, replaceValue))->Trim();
				}
				else { return "NULL/ "; }
			}
		}
		/// find Variables Local System from VarList ,all variable should have $ at the begining like $x or $x[1]
		if (argument->Contains("$")) {
			String^ tempInput = argument;
			while (argument->IndexOf("$") >= 0) {
				String^ oldValue = "";
				tempInput = argument->Substring(argument->IndexOf("$"), argument->Length - argument->IndexOf("$"));
				tempInput = findVarValue(tempInput, VarType::int_, oldValue);
				if (!Information::IsNumeric(tempInput)) {
					_plugin_logprint(Str2ConstChar(Environment::NewLine + tempInput));
					return "NULL/ ";
				}
				else {
					argument = (ReplaceAtIndex(argument, oldValue, tempInput))->Trim();
				}
			}
		}
		if ((argument->Contains("*")) || (argument->Contains("+")) || (argument->Contains("-")) || (argument->Contains("/"))) {  /// I will do it later
			while ((argument->Contains("*")) || (argument->Contains("+")) || (argument->Contains("-")) || (argument->Contains("/")))
			{
				if ((argument->StartsWith("*")) || (argument->StartsWith("+")) || (argument->StartsWith("/"))) {
					_plugin_logprint(Str2ConstChar(Environment::NewLine + "it begin with tokens + / *"));
					return "NULL/ ";
				}
				if (argument->StartsWith("-")) {  // in case it back negtive value or the number is negtive value
					return argument;
				}
				String^ tempInput = argument;
				String^ oldValue = "";
				tempInput = tokens(tempInput, oldValue);  /// we will get them as int value			
				if (tempInput->StartsWith("NULL/")) {
					_plugin_logprint(Str2ConstChar(Environment::NewLine + tempInput));
					return "NULL/ ";
				}
				else {
					argument = (ReplaceAtIndex(argument, oldValue, tempInput))->Trim();
				}
			}
		}
		break;
	}
	////////////			////////////			////////////
	case str:
	{
		if (argument->StartsWith("\"") && (argument->EndsWith("\""))) {  /// that mean all string is commaed 
			return argument;
		}
		if ((argument->IndexOf("{") >= 0) && (argument->IndexOf("}", argument->IndexOf("{")) >= 0)) {
			while (argument->IndexOf("{") >= 0) {
				String^ replaceValue = ""; String^ oldvalue;
				//if (argument->IndexOf("}") + 1 > argument->Length) {
				oldvalue = argument->Substring(argument->IndexOf("{"), argument->Length - argument->IndexOf("{"));
				oldvalue = oldvalue->Substring(0, oldvalue->IndexOf("}") + 1);
				//}
				//else
				//	oldvalue = argument->Substring(argument->IndexOf("{"), argument->Length);
				replaceValue = findScriptSystemVarValue(oldvalue);
				if (!replaceValue->StartsWith("NULL/")) {
					argument = ReplaceAtIndex(argument, oldvalue, replaceValue);
				}
			}
		}
		/// find Variables Local System from VarList ,all variable should have $ at the begining like $x or $x[1]
		if (argument->Contains("$")) {
			String^ tempInput = argument;
			while (argument->IndexOf("$") >= 0)
			{
				String^ oldValue = "";
				tempInput = argument->Substring(argument->IndexOf("$"), argument->Length - argument->IndexOf("$"));
				tempInput = findVarValue(tempInput, VarType::str, oldValue);
				if (tempInput->StartsWith("NULL/")) {
					_plugin_logprint(Str2ConstChar(tempInput));
					return "NULL/ ";
				}
				else {
					/// as it's str we return the int value to hex value 
					String^ oldv = "";
					if (CheckHexIsValid(tempInput, oldv)) {
						/*if (!tempInput->StartsWith("0x")) {
							tempInput = "0x" + tempInput->Trim();
						}*/
						if (Add0x) {
							if (!tempInput->Trim()->ToLower()->StartsWith("0x")) {  // check if there are 0x at begining 
								tempInput = "0x" + tempInput->Trim();
							}
						}
						tempInput = tempInput->Trim();
					}
					argument = ReplaceAtIndex(argument, oldValue, tempInput);
				}
			}
		}
		break;
	}
	////////////			////////////			////////////
	default:
		break;
	}


	return argument;
}

String^ returnSpaces(int SpaceNum) {  /// used for replaceValueBetweenBrackets to fill the gaps with spaces  
	String^ temp;
	for (int i = 0; i < SpaceNum; i++)
	{
		temp = temp + " ";
	}
	return temp;
}

/// used in cases $x[$z[3]] nasted variables (array)
String^ replaceValueBetweenBrackets(String^ input_) {
	String^ tempstr;
	if ((input_->Contains("[")) && (input_->Contains("]"))) {
		/// setx $c,$x[$z[3]]
		String^ Tinput = input_;
		int beginB = input_->LastIndexOf("[") + 1;
		tempstr = input_->Substring(beginB, input_->Length - beginB);
		int EndB = tempstr->IndexOf("]");
		tempstr = tempstr->Substring(0, EndB);

		while (beginB > 0)
		{
			if (tempstr->Trim()->StartsWith("$")) {
				String^ newValue = StrAnalyze(tempstr, VarType::int_);  /// we get int value after Analyze so we should make it hex 
				if (newValue->Contains("NULL/")) {
					return "NULL/";
				}
				if ((Information::IsNumeric(newValue))) {
					input_ = ReplaceAtIndex(input_, tempstr, str2Hex(newValue, VarType::int_, false));
					Tinput = ReplaceAtIndex(Tinput, "[" + tempstr + "]", returnSpaces((Tinput, "[" + tempstr + "]")->Length));
				}
			}
			else
			{
				Tinput = ReplaceAtIndex(Tinput, "[" + tempstr + "]", returnSpaces((Tinput, "[" + tempstr + "]")->Length));
			}
			int beginB = Tinput->LastIndexOf("[") + 1;
			if (beginB <= 0)
				break;
			tempstr = Tinput->Substring(beginB, Tinput->Length - beginB);
			int EndB = tempstr->IndexOf("]");
			tempstr = input_->Substring(beginB, EndB);

		}
	}
	return input_;
}

//String^ Get_adsValue(String^ input,int% EndB) {
//	bool found = false;
//	int ConstIndex = -1;
//	String^ ConstCommand;
//	if (input->StartsWith(" ")) {
//		_plugin_logprint(Str2ConstChar(Environment::NewLine + "space exist After ads."));
//		return "NULL/ ";
//	}
//		
//	String^ input_ = input->ToLower()->Trim();
//	array <String^>^ ListConst = { "exebase","modulebase","SectionSize","exefolderpath","exename",
//		"SectionBegin","SectionEnd" };
//	for (int i = 0; i < ListConst->Length ; i++)
//	{
//		if (input_->StartsWith(ListConst[i]->ToLower())) {
//			ConstIndex = i;
//			ConstCommand = ListConst[i];
//			found=true;
//			break;
//		}
//	}
//	if (!found)
//		return "NULL/ ";
//	switch (ConstIndex)
//	{
//	case exebase: {
//		///EndB = input->IndexOf(ConstCommand) + ConstCommand->Length; in case have begin with space
//		EndB = ConstCommand->Length;
//		return "0x" + duint2Hex(Script::Module::GetMainModuleBase());
//		break; 
//	}
//	case modulebase:  /// (name of module or address )
//	{
//		String^ findrest = input_->Substring(ConstCommand->Length, input_->Length - ConstCommand->Length);
//		if ((!findrest->Contains("(")) && (!findrest->Contains(")"))) {
//			_plugin_logprint(Str2ConstChar(Environment::NewLine + "no () After command(modulebase) of ads."));
//			EndB = ConstCommand->Length;
//			return "NULL/ ";
//		}
//		else
//		{
//			EndB = ConstCommand->Length + findrest->LastIndexOf(")") + 1;  // +1  to include ")"
//			findrest = findrest->Substring(1, findrest->LastIndexOf(")") -1 ); // remove ()
//			if ((findrest->Contains("$")) || (findrest->Contains("{")) || (findrest->Contains("ads."))) { // in case resolve value form x64dbg system
//				findrest = StrAnalyze(findrest, VarType::str, false);
//				//return  "0x" + duint2Hex(Script::Module::BaseFromName(Str2ConstChar(findrest)));
//				String^ intvalue;  /// first if it is hex value
//				if (CheckHexIsValid(findrest, intvalue) > 0) {  /// it mean it is address ( hex value)
//					return  "0x" + duint2Hex(Script::Module::BaseFromAddr(Str2int(intvalue)));  // resolve it as hex address 
//				}
//				else
//				{
//					return  "0x" + duint2Hex(Script::Module::BaseFromName(Str2ConstChar(findrest)));// resolve it as name of module 
//				}
//			}
//			String^ intvalue;
//			if (CheckHexIsValid(findrest, intvalue) > 0) {  /// it mean it is address ( hex value)
//				return  "0x" + duint2Hex(Script::Module::BaseFromAddr(Str2int(intvalue)));
//			}
//			else
//			{
//				_plugin_logprint(Str2ConstChar(Environment::NewLine + "can't resolve name or address after command(modulebase) of ads"));
//				return "NULL/ ";
//			}
//		}
//		break;
//	}		
//	case SectionSize:		// (address)
//	{
//		String^ findrest = input_->Substring(ConstCommand->Length, input_->Length - ConstCommand->Length);
//		if ((!findrest->Contains("(")) && (!findrest->Contains(")"))) {
//			_plugin_logprint(Str2ConstChar(Environment::NewLine + "no () After command(modulebase) of ads."));
//			EndB = ConstCommand->Length;
//			return "NULL/ ";
//		}
//		else
//		{
//			EndB = ConstCommand->Length + findrest->LastIndexOf(")") + 1;  // +1  to include ")"
//			findrest = findrest->Substring(1, findrest->LastIndexOf(")") - 1); // remove ()
//			if ((findrest->Contains("$")) || (findrest->Contains("{")) || (findrest->Contains("ads."))) { // in case resolve value form x64dbg system
//				findrest = StrAnalyze(findrest, VarType::str, false);
//				//return  "0x" + duint2Hex(Script::Module::SizeFromAddr(Str2ConstChar(findrest)));
//			}
//			String^ intvalue;
//			if (CheckHexIsValid(findrest, intvalue) > 0) {  /// it mean it is address ( hex value)
//				return  "0x" + duint2Hex(Script::Memory::GetSize(Hex2duint(findrest)));
//			}
//			else
//			{
//				_plugin_logprint(Str2ConstChar(Environment::NewLine + "can't resolve name or address after command(modulebase) of ads"));
//				return "NULL/ ";
//			}
//		}
//		break;
//	}
//	case exefolderpath: {
//		EndB = ConstCommand->Length;
//		char* path_ = new char[MAX_STRING_SIZE];
//		if (Script::Module::GetMainModulePath(path_))
//			return CharArr2Str(path_);
//		else
//			return "NULL/ ";
//		break;
//	}
//	case exename: {
//		EndB = ConstCommand->Length;
//		char* exename = new char[MAX_STRING_SIZE];
//		if (Script::Module::GetMainModuleName(exename))
//			return CharArr2Str(exename);
//		else
//			return "NULL/ ";
//		break;
//	}
//	case SectionBegin:  // ( address)
//	{
//		String^ findrest = input_->Substring(ConstCommand->Length, input_->Length - ConstCommand->Length);
//		if ((!findrest->Contains("(")) && (!findrest->Contains(")"))) {
//			_plugin_logprint(Str2ConstChar(Environment::NewLine + "no () After command(modulebase) of ads."));
//			EndB = ConstCommand->Length;
//			return "NULL/ ";
//		}
//		else
//		{
//			EndB = ConstCommand->Length + findrest->LastIndexOf(")") + 1;  // +1  to include ")"
//			findrest = findrest->Substring(1, findrest->LastIndexOf(")") - 1); // remove ()
//			if ((findrest->Contains("$")) || (findrest->Contains("{")) || (findrest->Contains("ads."))) { // in case resolve value form x64dbg system
//				findrest = StrAnalyze(findrest, VarType::str, false);				
//				String^ intvalue;
//				if (CheckHexIsValid(findrest, intvalue) > 0) {  /// it mean it is address ( hex value)
//					return  "0x" + duint2Hex(Script::Memory::GetBase(Hex2duint(findrest)));
//				}
//				else
//				{
//					_plugin_logprint(Str2ConstChar(Environment::NewLine + "can't resolve name or address after command(modulebase) of ads"));
//					return "NULL/ ";
//				}
//			}
//			String^ intvalue;
//			if (CheckHexIsValid(findrest, intvalue) > 0) {  /// it mean it is address ( hex value)
//				return  "0x" + duint2Hex(Script::Memory::GetBase(Hex2duint(findrest)));
//			}
//			else
//			{
//				_plugin_logprint(Str2ConstChar(Environment::NewLine + "can't resolve name or address after command(modulebase) of ads"));
//				return "NULL/ ";
//			}
//		}
//		break;
//	}
//		
//	case SectionEnd:
//	{
//		String^ findrest = input_->Substring(ConstCommand->Length, input_->Length - ConstCommand->Length);
//		if ((!findrest->Contains("(")) && (!findrest->Contains(")"))) {
//			_plugin_logprint(Str2ConstChar(Environment::NewLine + "no () After command(modulebase) of ads."));
//			EndB = ConstCommand->Length;
//			return "NULL/ ";
//		}
//		else
//		{
//			EndB = ConstCommand->Length + findrest->LastIndexOf(")") + 1;  // +1  to include ")"
//			findrest = findrest->Substring(1, findrest->LastIndexOf(")") - 1); // remove ()
//			if ((findrest->Contains("$")) || (findrest->Contains("{")) || (findrest->Contains("ads."))) { // in case resolve value form x64dbg system
//				findrest = StrAnalyze(findrest, VarType::str, false);
//				String^ intvalue;
//				if (CheckHexIsValid(findrest, intvalue) > 0) {  /// it mean it is address ( hex value)
//					duint EndSec = Script::Memory::GetSize(Hex2duint(findrest))+ Script::Memory::GetBase(Hex2duint(findrest));
//					return  "0x" + duint2Hex(EndSec);
//				}
//				else
//				{
//					_plugin_logprint(Str2ConstChar(Environment::NewLine + "can't resolve name or address after command(modulebase) of ads"));
//					return "NULL/ ";
//				}
//			}
//			String^ intvalue;
//			if (CheckHexIsValid(findrest, intvalue) > 0) {  /// it mean it is address ( hex value)
//				duint EndSec = Script::Memory::GetSize(Hex2duint(findrest)) + Script::Memory::GetBase(Hex2duint(findrest));
//				return  "0x" + duint2Hex(EndSec);
//			}
//			else
//			{
//				_plugin_logprint(Str2ConstChar(Environment::NewLine + "can't resolve name or address after command(modulebase) of ads"));
//				return "NULL/ ";
//			}
//		}
//		break;
//	}
//	default: {
//		_plugin_logprint(Str2ConstChar(Environment::NewLine + "no known command after ads."));
//		EndB = ConstCommand->Length;
//		return "NULL/ ";
//		break;
//	}
//	}
//
//
//}

String^ replace_ads(String^ input_) {
	String^ tempstr; int EndB; int beginB;
	String^ Tinput = input_->Trim()->ToLower();
	if (!Tinput->Contains("ads."))
		return input_;

	beginB = Tinput->IndexOf("ads.");
	while ((beginB >= 0) && (beginB < Tinput->Length))
	{
		beginB = Tinput->IndexOf("ads.");
		tempstr = Tinput->Substring(beginB, Tinput->Length - beginB);
		String^ restStr = tempstr->Substring(4, tempstr->Length - 4);
		String^ value_ = Get_adsValue(restStr, EndB);
		if (!value_->StartsWith("NULL/")) {
			restStr = restStr->Substring(0, EndB);
			Tinput = ReplaceAtIndex(Tinput, "ads." + restStr, value_);
			beginB += 4;
			if (!Tinput->Contains("ads."))
				return Tinput;
		}
		else
		{
			if (Interaction::MsgBox("error in resolve ads,continue or stop to fix the problem", MsgBoxStyle::YesNo, "Error") == MsgBoxResult::Yes)
				return input_;
			else
				return "NULL/";
		}
	}

	return input_;
}

String^ StrAnalyze(String^ input, VarType type_, bool Add0x) {  /// in case it int all value should be int , other wise it would be str and we add str to gather
	array <String^>^ breaks = { "*" ,"/" ,"+" ,"-" ,"$" ," " , "{" , "}" , "\"" };
	array <String^>^ token_ = { "*" ,"/" ,"+" ,"-" };
	array <String^>^ vars_ = { "$" ," " , "{" , "\"" };
	Generic::List <String^>^ StrHolderList = gcnew Generic::List <String^>;
	String^ temp = "";
	int begin_ = 0;
	if (input == "") {
		return input;
	}
	/// in cases like this  
	/// setx $c,$x[$z[3]]    here we have nested 
	String^ retvalue = replaceValueBetweenBrackets(input);
	if (!retvalue->Contains("NULL/"))
		input = retvalue;
	else
		return "NULL/";
	///
	// case we have ads.xxxx
	input = replace_ads(input);
	///
	// in case we got directly hex value or numerical value 
	String^ intValue;
	if (CheckHexIsValid(input, intValue) != 0) {
		if (type_ == VarType::int_)
			return intValue;
		if (type_ == VarType::str) {
			if (Add0x) {
				if (input->Trim()->ToLower()->StartsWith("0x")) {
					return input->Trim();
				}
				else
				{
					return "0x" + input->Trim();
				}
			}
			else
			{
				if (input->Trim()->ToLower()->StartsWith("0x")) {
					return input->Trim()->Substring(2, input->Trim()->Length - 2);   /// remove 0x
				}
				else
				{
					return  input->Trim();
				}
			}
		}
	}
	///
	if (Array::IndexOf(vars_, input->Substring(0, 1)) >= 0) {/// if (i=0) begin with vars defenations this we need to add it 

		if (input->Substring(0, 1) == " ") {  /// in case calc str or array we need spaces
			if (type_ != VarType::int_) {
				temp = temp + input->Substring(0, 1);
				begin_ += 1;
			}
			else
			{
				temp = input;
				temp = temp->Trim();   /// case int calculation
			}
		}
		if (input->Substring(0, 1) == "$") {
			temp = temp + input->Substring(0, 1);
			begin_ += 1;
		}

		if (input->Substring(0, 1) == "{") {
			if (type_ != VarType::int_) {  /// case array or str 
				if (input->IndexOf("}") < 0) { // if we don't find { then we consider it as char in string
					temp = temp + input->Substring(0, 1);
					begin_ += 1;
				}
				else
				{
					//temp = temp + input->Substring(0, input->IndexOf("}") + 1);  /// we collect all {...}
					StrHolderList->Add(input->Substring(0, input->IndexOf("}") + 1));
					begin_ = input->IndexOf("}") + 1;
				}
			}
			else {  /// case it is int , so if we don't find next } then this wrong 
				if (input->IndexOf("}") < 0) {
					return "NULL/ ";
				}
				else
				{
					//temp = temp + input->Substring(0, input->IndexOf("}") + 1);  /// we collect all {...}
					StrHolderList->Add(input->Substring(0, input->IndexOf("}") + 1));
					begin_ = input->IndexOf("}") + 1;
				}
			}
		}

		if (input->Substring(0, 1) == "\"") {  /// if it's "
			if (type_ != VarType::int_) {  /// case array or str 
				if (input->IndexOf("\"", 1, input->Length - 1) < 0) { // if we don't find " then we consider it as char in string
					temp = temp + input->Substring(0, 1);
					begin_ += 1;
				}
				else
				{
					//temp = temp + input->Substring(0, input->IndexOf("\"", 1, 1) + 1);  /// we collect all "..."
					StrHolderList->Add(input->Substring(0, input->IndexOf("\"", 1, input->Length - 1) + 1));
					begin_ = input->IndexOf("\"", 1, input->Length - 1) + 1;
				}
			}
			else {  /// case it is int , " not acceptable				
				return "NULL/ ";
			}
		}

	}

	for (int i = begin_; i < input->Length; i++)
	{
		if (Array::IndexOf(breaks, input->Substring(i, 1)) < 0) {
			temp = temp + input->Substring(i, 1);
		}
		else
		{
			if (input->Substring(i, 1) == " ") {  /// in case calc str or array we need spaces
				if (type_ != VarType::int_) {
					temp = temp + input->Substring(i, 1);
					if (i + 1 < input->Length) {
						if (input->Substring(i + 1, 1) != " ") {
							StrHolderList->Add(temp);
							temp = "";   /// rest temp	
						}
					}
				}
			}
			if (input->Substring(i, 1) == "$") {
				if (temp != "") {
					StrHolderList->Add(temp);
					temp = "";
				}
				temp = temp + input->Substring(i, 1);
			}

			if (input->Substring(i, 1) == "{") {
				if (type_ != VarType::int_) {  /// case array or str 
					if (input->IndexOf("}", i, input->Length - i) < 0) { // if we don't find { then we consider it as char in string
						temp = temp + input->Substring(i, 1);
					}
					else
					{
						temp = temp + input->Substring(i, (input->IndexOf("}", i, input->Length - i) - i) + 1);  /// we collect all {...}
						if (temp != "") {
							StrHolderList->Add(temp);
							temp = "";
						}
						i = input->IndexOf("}", i, input->Length - i) + 1;
						if (i >= input->Length) {
							break;
						}
					}
				}
				else {  /// case it is int , so if we don't find next } then this wrong 
					if (input->IndexOf("}", i, input->Length - i) < 0) {
						return "NULL/ ";
					}
					else
					{
						temp = temp + input->Substring(i, (input->IndexOf("}", i, input->Length - i) - i) + 1);  /// we collect all {...}
						if (temp != "") {
							StrHolderList->Add(temp);
							temp = "";
						}
						i = input->IndexOf("}", i, input->Length - i) + 1;
						if (i >= input->Length) {
							break;
						}
					}
				}
			}

			if (input->Substring(i, 1) == "\"") {  /// if it's "
				if (type_ != VarType::int_) {  /// case array or str 
					if (input->IndexOf("\"", i, input->Length - i) < 0) { // if we don't find " then we consider it as char in string
						temp = temp + input->Substring(i, 1);
					}
					else
					{
						int cc = input->IndexOf("\"", i + 1, input->Length - (i + 1));
						if (cc<0)
							return "NULL/ ";
						temp = temp + input->Substring(i, (cc - i) + 1);  /// we collect all "..."
						if (temp != "") {
							StrHolderList->Add(temp);
							temp = "";
						}
						i = cc + 1;
						if (i >= input->Length) {
							break;
						}
					}
				}
				else {  /// case it is int , " not acceptable				
					return "NULL/ ";
				}
			}
			if (Array::IndexOf(token_, input->Substring(i, 1)) >= 0) {
				if ((temp != "") || ((temp == nullptr))) {
					StrHolderList->Add(temp);
				}
				StrHolderList->Add(input->Substring(i, 1));
				temp = "";
			}

		}
		if (i == input->Length - 1) {
			StrHolderList->Add(temp);
			temp = "";
		}
	}
	if (temp->Length == 1) {  // case enter 1 special char
		StrHolderList->Add(temp);
		temp = "";   /// rest temp	
	}
	////////////// now we work on evaluate StrHolderList array
	String^ StrHolder = "";
	switch (type_)
	{
	case int_:
	{
		for (int i = 0; i < StrHolderList->Count; i++)
		{
			if ((StrHolderList[i] != "") || (StrHolderList[i] != " ")) {
				if (Array::IndexOf(token_, StrHolderList[i]) < 0) {
					StrHolderList[i] = GetArgValueByType(StrHolderList[i], VarType::int_);
					StrHolder = StrHolder + StrHolderList[i]->Trim(); // as it's int we can clean unneeded spaces
				}
				else
				{
					StrHolder = StrHolder + StrHolderList[i];
				}
			}
		}
		if (!Information::IsNumeric(StrHolder)) {  ///// that mean all string has been solved as int no need for more analyze
			StrHolder = GetArgValueByType(StrHolder, VarType::int_);
		}
		break;
	}
	case str:
	{
		for (int i = 0; i < StrHolderList->Count; i++)
		{
			if (Array::IndexOf(token_, StrHolderList[i]) < 0) {
				StrHolderList[i] = GetArgValueByType(StrHolderList[i], VarType::str, Add0x);
				StrHolder = StrHolder + StrHolderList[i];
			}
			else
			{
				StrHolder = StrHolder + StrHolderList[i];
			}

		}
		break;
	}
	default:
		break;
	}

	return StrHolder;
}


