﻿using AutomationTool;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using UnrealBuildTool;

namespace BuildGraph.Tasks
{
	/// <summary>
	/// Parameters for the log task
	/// </summary>
	public class LogTaskParameters
	{
		/// <summary>
		/// Message to print out
		/// </summary>
		[TaskParameter(Optional = true)]
		public string Message;

		/// <summary>
		/// If specified, causes the given list of files to be printed after the given message.
		/// </summary>
		[TaskParameter(Optional = true)]
		public string Files;

		/// <summary>
		/// If specified, causes the contents of the given files to be printed out.
		/// </summary>
		[TaskParameter(Optional = true)]
		public bool IncludeContents;
	}

	/// <summary>
	/// Print a message (and other optional diagnostic information) to the output log
	/// </summary>
	[TaskElement("Log", typeof(LogTaskParameters))]
	public class LogTask : CustomTask
	{
		/// <summary>
		/// Parameters for the task
		/// </summary>
		LogTaskParameters Parameters;

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="InParameters">Parameters for the task</param>
		public LogTask(LogTaskParameters InParameters)
		{
			Parameters = InParameters;
		}

		/// <summary>
		/// Execute the task.
		/// </summary>
		/// <param name="Job">Information about the current job</param>
		/// <param name="BuildProducts">Set of build products produced by this node.</param>
		/// <param name="TagNameToFileSet">Mapping from tag names to the set of files they include</param>
		/// <returns>True if the task succeeded</returns>
		public override bool Execute(JobContext Job, HashSet<FileReference> BuildProducts, Dictionary<string, HashSet<FileReference>> TagNameToFileSet)
		{
			// Print the message
			if(!String.IsNullOrEmpty(Parameters.Message))
			{
				CommandUtils.Log(Parameters.Message);
			}

			// Print the contents of the given tag, if specified
			if(!String.IsNullOrEmpty(Parameters.Files))
			{
				HashSet<FileReference> Files = ResolveFilespec(CommandUtils.RootDirectory, Parameters.Files, TagNameToFileSet);
				foreach(FileReference File in Files.OrderBy(x => x.FullName))
				{
					CommandUtils.Log("  {0}", File.FullName);
					if(Parameters.IncludeContents)
					{
						foreach(string Line in System.IO.File.ReadAllLines(File.FullName))
						{
							CommandUtils.Log("    {0}", Line);
						}
					}
				}
			}

			return true;
		}

		/// <summary>
		/// Output this task out to an XML writer.
		/// </summary>
		public override void Write(XmlWriter Writer)
		{
			Write(Writer, Parameters);
		}
	}
}