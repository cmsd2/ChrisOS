/******************************************************************************
 *
 * Module Name: aslfold - Constant folding
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2014, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights. You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code. No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision. In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change. Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee. Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution. In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE. ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT, ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES. INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS. INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government. In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#include "aslcompiler.h"
#include "aslcompiler.y.h"
#include "amlcode.h"

#include "acdispat.h"
#include "acparser.h"

#define _COMPONENT          ACPI_COMPILER
        ACPI_MODULE_NAME    ("aslfold")

/* Local prototypes */

static ACPI_STATUS
OpcAmlEvaluationWalk1 (
    ACPI_PARSE_OBJECT       *Op,
    UINT32                  Level,
    void                    *Context);

static ACPI_STATUS
OpcAmlEvaluationWalk2 (
    ACPI_PARSE_OBJECT       *Op,
    UINT32                  Level,
    void                    *Context);

static ACPI_STATUS
OpcAmlCheckForConstant (
    ACPI_PARSE_OBJECT       *Op,
    UINT32                  Level,
    void                    *Context);

static void
OpcUpdateIntegerNode (
    ACPI_PARSE_OBJECT       *Op,
    UINT64                  Value);

static ACPI_STATUS
TrTransformToStoreOp (
    ACPI_PARSE_OBJECT       *Op,
    ACPI_WALK_STATE         *WalkState);

static ACPI_STATUS
TrSimpleConstantReduction (
    ACPI_PARSE_OBJECT       *Op,
    ACPI_WALK_STATE         *WalkState);

static void
TrInstallReducedConstant (
    ACPI_PARSE_OBJECT       *Op,
    ACPI_OPERAND_OBJECT     *ObjDesc);


/*******************************************************************************
 *
 * FUNCTION:    OpcAmlConstantWalk
 *
 * PARAMETERS:  ASL_WALK_CALLBACK
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Reduce an Op and its subtree to a constant if possible
 *
 ******************************************************************************/

ACPI_STATUS
OpcAmlConstantWalk (
    ACPI_PARSE_OBJECT       *Op,
    UINT32                  Level,
    void                    *Context)
{
    ACPI_WALK_STATE         *WalkState;
    ACPI_STATUS             Status = AE_OK;


    if (Op->Asl.CompileFlags == 0)
    {
        return (AE_OK);
    }

    /*
     * Only interested in subtrees that could possibly contain
     * expressions that can be evaluated at this time
     */
    if ((!(Op->Asl.CompileFlags & NODE_COMPILE_TIME_CONST)) ||
          (Op->Asl.CompileFlags & NODE_IS_TARGET))
    {
        return (AE_OK);
    }

    /* Create a new walk state */

    WalkState = AcpiDsCreateWalkState (0, NULL, NULL, NULL);
    if (!WalkState)
    {
        return (AE_NO_MEMORY);
    }

    WalkState->NextOp = NULL;
    WalkState->Params = NULL;

    /*
     * Examine the entire subtree -- all nodes must be constants
     * or type 3/4/5 opcodes
     */
    Status = TrWalkParseTree (Op, ASL_WALK_VISIT_DOWNWARD,
        OpcAmlCheckForConstant, NULL, WalkState);

    /*
     * Did we find an entire subtree that contains all constants
     * and type 3/4/5 opcodes?
     */
    switch (Status)
    {
    case AE_OK:

        Status = TrSimpleConstantReduction (Op, WalkState);
        break;

    case AE_CTRL_RETURN_VALUE:

        Status = TrTransformToStoreOp (Op, WalkState);
        break;

    case AE_TYPE:

        AcpiDsDeleteWalkState (WalkState);
        return (AE_OK);

    default:
        AcpiDsDeleteWalkState (WalkState);
        return (AE_OK);
    }

    if (ACPI_FAILURE (Status))
    {
        DbgPrint (ASL_PARSE_OUTPUT, "Cannot resolve, %s\n",
            AcpiFormatException (Status));

        /* We could not resolve the subtree for some reason */

        AslError (ASL_ERROR, ASL_MSG_CONSTANT_EVALUATION, Op,
            Op->Asl.ParseOpName);

        /* Set the subtree value to ZERO anyway. Eliminates further errors */

        OpcUpdateIntegerNode (Op, 0);
    }

    AcpiDsDeleteWalkState (WalkState);
    return (AE_CTRL_DEPTH);
}


/*******************************************************************************
 *
 * FUNCTION:    OpcAmlCheckForConstant
 *
 * PARAMETERS:  ASL_WALK_CALLBACK
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Check one Op for a type 3/4/5 AML opcode
 *
 ******************************************************************************/

static ACPI_STATUS
OpcAmlCheckForConstant (
    ACPI_PARSE_OBJECT       *Op,
    UINT32                  Level,
    void                    *Context)
{
    ACPI_WALK_STATE         *WalkState = Context;
    ACPI_STATUS             Status = AE_OK;


    WalkState->Op = Op;
    WalkState->Opcode = Op->Common.AmlOpcode;
    WalkState->OpInfo = AcpiPsGetOpcodeInfo (Op->Common.AmlOpcode);

    DbgPrint (ASL_PARSE_OUTPUT, "[%.4d] Opcode: %12.12s ",
        Op->Asl.LogicalLineNumber, Op->Asl.ParseOpName);

    /*
     * These opcodes do not appear in the OpcodeInfo table, but
     * they represent constants, so abort the constant walk now.
     */
    if ((WalkState->Opcode == AML_RAW_DATA_BYTE) ||
        (WalkState->Opcode == AML_RAW_DATA_WORD) ||
        (WalkState->Opcode == AML_RAW_DATA_DWORD) ||
        (WalkState->Opcode == AML_RAW_DATA_QWORD))
    {
        DbgPrint (ASL_PARSE_OUTPUT, "RAW DATA");
        Status = AE_OK;
        goto CleanupAndExit;
    }

    /* Type 3/4/5 opcodes have the AML_CONSTANT flag set */

    if (!(WalkState->OpInfo->Flags & AML_CONSTANT))
    {
        /* Not 3/4/5 opcode, but maybe can convert to STORE */

        if (Op->Asl.CompileFlags & NODE_IS_TARGET)
        {
            DbgPrint (ASL_PARSE_OUTPUT,
                "**** Valid Target, transform to Store ****\n");
            return (AE_CTRL_RETURN_VALUE);
        }

        /* Expression cannot be reduced */

        DbgPrint (ASL_PARSE_OUTPUT,
            "**** Not a Type 3/4/5 opcode (%s) ****",
             Op->Asl.ParseOpName);

        Status = AE_TYPE;
        goto CleanupAndExit;
    }

    /* Debug output */

    DbgPrint (ASL_PARSE_OUTPUT, "TYPE_345");

    if (Op->Asl.CompileFlags & NODE_IS_TARGET)
    {
        if (Op->Asl.ParseOpcode == PARSEOP_ZERO)
        {
            DbgPrint (ASL_PARSE_OUTPUT, "%-16s", " NULL TARGET");
        }
        else
        {
            DbgPrint (ASL_PARSE_OUTPUT, "%-16s", " VALID TARGET");
        }
    }
    if (Op->Asl.CompileFlags & NODE_IS_TERM_ARG)
    {
        DbgPrint (ASL_PARSE_OUTPUT, "%-16s", " TERMARG");
    }

CleanupAndExit:

    /* Dump the node compile flags also */

    TrPrintNodeCompileFlags (Op->Asl.CompileFlags);
    DbgPrint (ASL_PARSE_OUTPUT, "\n");
    return (Status);
}


/*******************************************************************************
 *
 * FUNCTION:    TrSimpleConstantReduction
 *
 * PARAMETERS:  Op                  - Parent operator to be transformed
 *              WalkState           - Current walk state
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Reduce an entire AML operation to a single constant. The
 *              operation must not have a target operand.
 *
 *              Add (32,64) --> 96
 *
 ******************************************************************************/

static ACPI_STATUS
TrSimpleConstantReduction (
    ACPI_PARSE_OBJECT       *Op,
    ACPI_WALK_STATE         *WalkState)
{
    ACPI_PARSE_OBJECT       *RootOp;
    ACPI_PARSE_OBJECT       *OriginalParentOp;
    ACPI_OPERAND_OBJECT     *ObjDesc;
    ACPI_STATUS             Status;


    DbgPrint (ASL_PARSE_OUTPUT,
        "Simple subtree constant reduction, operator to constant\n");

    /* Allocate a new temporary root for this subtree */

    RootOp = TrAllocateNode (PARSEOP_INTEGER);
    if (!RootOp)
    {
        return (AE_NO_MEMORY);
    }

    RootOp->Common.AmlOpcode = AML_INT_EVAL_SUBTREE_OP;

    OriginalParentOp = Op->Common.Parent;
    Op->Common.Parent = RootOp;

    /* Hand off the subtree to the AML interpreter */

    WalkState->CallerReturnDesc = &ObjDesc;

    Status = TrWalkParseTree (Op, ASL_WALK_VISIT_TWICE,
        OpcAmlEvaluationWalk1, OpcAmlEvaluationWalk2, WalkState);

    /* Restore original parse tree */

    Op->Common.Parent = OriginalParentOp;

    if (ACPI_FAILURE (Status))
    {
        DbgPrint (ASL_PARSE_OUTPUT,
            "Constant Subtree evaluation(1), %s\n",
            AcpiFormatException (Status));
        return (Status);
    }

    /* Get the final result */

    Status = AcpiDsResultPop (&ObjDesc, WalkState);
    if (ACPI_FAILURE (Status))
    {
        DbgPrint (ASL_PARSE_OUTPUT,
            "Constant Subtree evaluation(2), %s\n",
            AcpiFormatException (Status));
        return (Status);
    }

    TrInstallReducedConstant (Op, ObjDesc);

    UtSetParseOpName (Op);
    Op->Asl.Child = NULL;
    return (AE_OK);
}


/*******************************************************************************
 *
 * FUNCTION:    TrTransformToStoreOp
 *
 * PARAMETERS:  Op                  - Parent operator to be transformed
 *              WalkState           - Current walk state
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Transforms a single AML operation with a constant and target
 *              to a simple store operation:
 *
 *              Add (32,64,DATA) --> Store (96,DATA)
 *
 ******************************************************************************/

static ACPI_STATUS
TrTransformToStoreOp (
    ACPI_PARSE_OBJECT       *Op,
    ACPI_WALK_STATE         *WalkState)
{
    ACPI_PARSE_OBJECT       *OriginalTarget;
    ACPI_PARSE_OBJECT       *NewTarget;
    ACPI_PARSE_OBJECT       *Child1;
    ACPI_PARSE_OBJECT       *Child2;
    ACPI_OPERAND_OBJECT     *ObjDesc;
    ACPI_PARSE_OBJECT       *NewParent;
    ACPI_PARSE_OBJECT       *OriginalParent;
    ACPI_STATUS             Status;


    DbgPrint (ASL_PARSE_OUTPUT,
        "Reduction/Transform to StoreOp: Store(Constant, Target)\n");

    /* Extract the operands */

    Child1 = Op->Asl.Child;
    Child2 = Child1->Asl.Next;

    /*
     * Special case for DIVIDE -- it has two targets. The first
     * is for the remainder and if present, we will not attempt
     * to reduce the expression.
     */
    if (Op->Asl.ParseOpcode == PARSEOP_DIVIDE)
    {
        Child2 = Child2->Asl.Next;
        if (Child2->Asl.ParseOpcode != PARSEOP_ZERO)
        {
            DbgPrint (ASL_PARSE_OUTPUT,
                "Cannot reduce DIVIDE - has two targets\n\n");
            return (AE_OK);
        }
    }

    /*
     * Create a NULL (zero) target so that we can use the
     * interpreter to evaluate the expression.
     */
    NewTarget = TrCreateNullTarget ();
    NewTarget->Common.AmlOpcode = AML_INT_NAMEPATH_OP;

    /* Handle one-operand cases (NOT, TOBCD, etc.) */

    if (!Child2->Asl.Next)
    {
        Child2 = Child1;
    }

    /* Link in new NULL target as the last operand */

    OriginalTarget = Child2->Asl.Next;
    Child2->Asl.Next = NewTarget;
    NewTarget->Asl.Parent = OriginalTarget->Asl.Parent;

    NewParent = TrAllocateNode (PARSEOP_INTEGER);
    NewParent->Common.AmlOpcode = AML_INT_EVAL_SUBTREE_OP;

    OriginalParent = Op->Common.Parent;
    Op->Common.Parent = NewParent;

    /* Hand off the subtree to the AML interpreter */

    WalkState->CallerReturnDesc = &ObjDesc;

    Status = TrWalkParseTree (Op, ASL_WALK_VISIT_TWICE,
        OpcAmlEvaluationWalk1, OpcAmlEvaluationWalk2, WalkState);
    if (ACPI_FAILURE (Status))
    {
        DbgPrint (ASL_PARSE_OUTPUT,
            "Constant Subtree evaluation(3), %s\n",
            AcpiFormatException (Status));
        return (Status);
    }

    /* Get the final result */

    Status = AcpiDsResultPop (&ObjDesc, WalkState);
    if (ACPI_FAILURE (Status))
    {
        DbgPrint (ASL_PARSE_OUTPUT,
            "Constant Subtree evaluation(4), %s\n",
            AcpiFormatException (Status));
        return (Status);
    }

    TrInstallReducedConstant (Child1, ObjDesc);

    /* Convert operator to STORE */

    Op->Asl.ParseOpcode = PARSEOP_STORE;
    Op->Asl.AmlOpcode = AML_STORE_OP;
    UtSetParseOpName (Op);
    Op->Common.Parent = OriginalParent;

    /* Truncate any subtree expressions, they have been evaluated */

    Child1->Asl.Child = NULL;
    Child2->Asl.Child = NULL;

    /* First child is the folded constant */

    /* Second child will be the target */

    Child1->Asl.Next = OriginalTarget;
    return (AE_OK);
}


/*******************************************************************************
 *
 * FUNCTION:    TrInstallReducedConstant
 *
 * PARAMETERS:  Op                  - Parent operator to be transformed
 *              ObjDesc             - Reduced constant to be installed
 *
 * RETURN:      None
 *
 * DESCRIPTION: Transform the original operator to a simple constant.
 *              Handles Integers, Strings, and Buffers.
 *
 ******************************************************************************/

static void
TrInstallReducedConstant (
    ACPI_PARSE_OBJECT       *Op,
    ACPI_OPERAND_OBJECT     *ObjDesc)
{
    ACPI_PARSE_OBJECT       *RootOp;


    TotalFolds++;
    AslError (ASL_OPTIMIZATION, ASL_MSG_CONSTANT_FOLDED, Op,
        Op->Asl.ParseOpName);

    /*
     * Because we know we executed type 3/4/5 opcodes above, we know that
     * the result must be either an Integer, String, or Buffer.
     */
    switch (ObjDesc->Common.Type)
    {
    case ACPI_TYPE_INTEGER:

        OpcUpdateIntegerNode (Op, ObjDesc->Integer.Value);

        DbgPrint (ASL_PARSE_OUTPUT,
            "Constant expression reduced to (%s) %8.8X%8.8X\n\n",
            Op->Asl.ParseOpName,
            ACPI_FORMAT_UINT64 (Op->Common.Value.Integer));
        break;

    case ACPI_TYPE_STRING:

        Op->Asl.ParseOpcode = PARSEOP_STRING_LITERAL;
        Op->Common.AmlOpcode = AML_STRING_OP;
        Op->Asl.AmlLength = ACPI_STRLEN (ObjDesc->String.Pointer) + 1;
        Op->Common.Value.String = ObjDesc->String.Pointer;

        DbgPrint (ASL_PARSE_OUTPUT,
            "Constant expression reduced to (STRING) %s\n\n",
            Op->Common.Value.String);

        break;

    case ACPI_TYPE_BUFFER:

        Op->Asl.ParseOpcode = PARSEOP_BUFFER;
        Op->Common.AmlOpcode = AML_BUFFER_OP;
        Op->Asl.CompileFlags = NODE_AML_PACKAGE;
        UtSetParseOpName (Op);

        /* Child node is the buffer length */

        RootOp = TrAllocateNode (PARSEOP_INTEGER);

        RootOp->Asl.AmlOpcode = AML_DWORD_OP;
        RootOp->Asl.Value.Integer = ObjDesc->Buffer.Length;
        RootOp->Asl.Parent = Op;

        (void) OpcSetOptimalIntegerSize (RootOp);

        Op->Asl.Child = RootOp;
        Op = RootOp;
        UtSetParseOpName (Op);

        /* Peer to the child is the raw buffer data */

        RootOp = TrAllocateNode (PARSEOP_RAW_DATA);
        RootOp->Asl.AmlOpcode = AML_RAW_DATA_BUFFER;
        RootOp->Asl.AmlLength = ObjDesc->Buffer.Length;
        RootOp->Asl.Value.String = (char *) ObjDesc->Buffer.Pointer;
        RootOp->Asl.Parent = Op->Asl.Parent;

        Op->Asl.Next = RootOp;
        Op = RootOp;

        DbgPrint (ASL_PARSE_OUTPUT,
            "Constant expression reduced to (BUFFER) length %X\n\n",
            ObjDesc->Buffer.Length);
        break;

    default:

        printf ("Unsupported return type: %s\n",
            AcpiUtGetObjectTypeName (ObjDesc));
        break;
    }
}


/*******************************************************************************
 *
 * FUNCTION:    OpcUpdateIntegerNode
 *
 * PARAMETERS:  Op                  - Current parse object
 *              Value               - Value for the integer op
 *
 * RETURN:      None
 *
 * DESCRIPTION: Update node to the correct Integer type and value
 *
 ******************************************************************************/

static void
OpcUpdateIntegerNode (
    ACPI_PARSE_OBJECT       *Op,
    UINT64                  Value)
{

    Op->Common.Value.Integer = Value;

    /*
     * The AmlLength is used by the parser to indicate a constant,
     * (if non-zero). Length is either (1/2/4/8)
     */
    switch (Op->Asl.AmlLength)
    {
    case 1:

        TrUpdateNode (PARSEOP_BYTECONST, Op);
        Op->Asl.AmlOpcode = AML_RAW_DATA_BYTE;
        break;

    case 2:

        TrUpdateNode (PARSEOP_WORDCONST, Op);
        Op->Asl.AmlOpcode = AML_RAW_DATA_WORD;
        break;

    case 4:

        TrUpdateNode (PARSEOP_DWORDCONST, Op);
        Op->Asl.AmlOpcode = AML_RAW_DATA_DWORD;
        break;

    case 8:

        TrUpdateNode (PARSEOP_QWORDCONST, Op);
        Op->Asl.AmlOpcode = AML_RAW_DATA_QWORD;
        break;

    case 0:
    default:

        OpcSetOptimalIntegerSize (Op);
        TrUpdateNode (PARSEOP_INTEGER, Op);
        break;
    }

    Op->Asl.AmlLength = 0;
}


/*******************************************************************************
 *
 * FUNCTION:    OpcAmlEvaluationWalk1
 *
 * PARAMETERS:  ASL_WALK_CALLBACK
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Descending callback for AML execution of constant subtrees
 *
 ******************************************************************************/

static ACPI_STATUS
OpcAmlEvaluationWalk1 (
    ACPI_PARSE_OBJECT       *Op,
    UINT32                  Level,
    void                    *Context)
{
    ACPI_WALK_STATE         *WalkState = Context;
    ACPI_STATUS             Status;
    ACPI_PARSE_OBJECT       *OutOp;


    WalkState->Op = Op;
    WalkState->Opcode = Op->Common.AmlOpcode;
    WalkState->OpInfo = AcpiPsGetOpcodeInfo (Op->Common.AmlOpcode);

    /* Copy child pointer to Arg for compatibility with Interpreter */

    if (Op->Asl.Child)
    {
        Op->Common.Value.Arg = Op->Asl.Child;
    }

    /* Call AML dispatcher */

    Status = AcpiDsExecBeginOp (WalkState, &OutOp);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("Constant interpretation failed - %s\n",
            AcpiFormatException (Status));
    }

    return (Status);
}


/*******************************************************************************
 *
 * FUNCTION:    OpcAmlEvaluationWalk2
 *
 * PARAMETERS:  ASL_WALK_CALLBACK
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Ascending callback for AML execution of constant subtrees
 *
 ******************************************************************************/

static ACPI_STATUS
OpcAmlEvaluationWalk2 (
    ACPI_PARSE_OBJECT       *Op,
    UINT32                  Level,
    void                    *Context)
{
    ACPI_WALK_STATE         *WalkState = Context;
    ACPI_STATUS             Status;


    WalkState->Op = Op;
    WalkState->Opcode = Op->Common.AmlOpcode;
    WalkState->OpInfo = AcpiPsGetOpcodeInfo (Op->Common.AmlOpcode);

    /* Copy child pointer to Arg for compatibility with Interpreter */

    if (Op->Asl.Child)
    {
        Op->Common.Value.Arg = Op->Asl.Child;
    }

    /* Call AML dispatcher */

    Status = AcpiDsExecEndOp (WalkState);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsPrintf ("Constant interpretation failed - %s\n",
            AcpiFormatException (Status));
    }

    return (Status);
}
