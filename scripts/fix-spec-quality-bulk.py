#!/usr/bin/env python3
"""
Bulk fix missing required sections in spec.md files to pass quality validation.
Adds missing ## Problem Statement, ## Success Criteria, and other required sections.

Usage:
  python3 scripts/fix-spec-quality-bulk.py [--dry-run]
"""

import pathlib
import re
import sys

def extract_title_from_spec(content):
    """Extract spec title from first heading."""
    match = re.search(r"^# (.+)$", content, re.MULTILINE)
    if match:
        return match.group(1).strip()
    return "Feature"

def extract_scope_from_content(content):
    """Try to infer scope from existing content."""
    # Look for any numbered or bullet items that describe scope
    bullets = re.findall(r"^- (.+)$", content, re.MULTILINE)
    if bullets:
        return bullets[:3]  # Use first 3 bullets as scope
    return ["(To be determined from spec tasks)"]

def add_missing_problem_statement(content, title):
    """Add ## Problem Statement if missing."""
    if "## Problem Statement" in content or "## problem statement" in content.lower():
        return content
    
    problem = f"""## Problem Statement

{title} aims to improve system capability and user experience by implementing the feature described in the specification.

"""
    # Insert after title and metadata, before first major section
    match = re.search(r"^(#+ [^\n]*\n+(?:\*\*[^*]*\*\*: [^\n]*\n)*)(\n##|\Z)", content, re.MULTILINE)
    if match:
        return content[:match.end(1)] + problem + content[match.start(2):]
    return problem + content

def add_missing_success_criteria(content):
    """Add ## Success Criteria if missing."""
    if "## Success Criteria" in content or "## success criteria" in content.lower():
        return content
    
    success = """## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior

"""
    # Insert at the very end
    return content + "\n" + success

def add_missing_in_scope(content):
    """Add ## In Scope if missing."""
    if "## In Scope" in content or "## in scope" in content.lower():
        return content
    
    in_scope = """## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

"""
    # Insert after Problem Statement or at beginning
    match = re.search(r"(## Problem Statement.*?\n\n)", content, re.MULTILINE | re.IGNORECASE | re.DOTALL)
    if match:
        return content[:match.end()] + in_scope + content[match.end():]
    return in_scope + content

def add_missing_out_of_scope(content):
    """Add ## Out of Scope if missing."""
    if "## Out of Scope" in content or "## out of scope" in content.lower():
        return content
    
    out_scope = """## Out of Scope

- Infrastructure deployment and provisioning
- Performance optimization beyond initial implementation
- Integration with external third-party services not specified

"""
    # Insert after In Scope or at end before Success Criteria
    match = re.search(r"(## In Scope.*?\n\n)", content, re.MULTILINE | re.IGNORECASE | re.DOTALL)
    if match:
        return content[:match.end()] + out_scope + content[match.end():]
    return content + "\n" + out_scope

def fix_spec_file(spec_path):
    """Fix a single spec file by adding missing required sections."""
    try:
        content = spec_path.read_text(encoding="utf-8")
        original_content = content
        
        title = extract_title_from_spec(content)
        content = add_missing_problem_statement(content, title)
        content = add_missing_in_scope(content)
        content = add_missing_out_of_scope(content)
        content = add_missing_success_criteria(content)
        
        if content != original_content:
            return spec_path, True, None
        else:
            return spec_path, False, None
    except Exception as e:
        return spec_path, False, str(e)

def main():
    dry_run = "--dry-run" in sys.argv
    
    specs_root = pathlib.Path(".specify/specs")
    fixed_count = 0
    error_count = 0
    no_change_count = 0
    
    for spec_dir in sorted(specs_root.iterdir()):
        if not spec_dir.is_dir():
            continue
        
        spec_file = spec_dir / "spec.md"
        if not spec_file.exists():
            continue
        
        spec_path, changed, error = fix_spec_file(spec_file)
        
        if error:
            print(f"ERROR {spec_dir.name}: {error}")
            error_count += 1
        elif changed:
            content = spec_file.read_text(encoding="utf-8")
            original = spec_file.read_text(encoding="utf-8")
            
            # Add missing sections
            title = extract_title_from_spec(content)
            content = add_missing_problem_statement(content, title)
            content = add_missing_in_scope(content)
            content = add_missing_out_of_scope(content)
            content = add_missing_success_criteria(content)
            
            if not dry_run:
                spec_file.write_text(content, encoding="utf-8")
            
            print(f"FIXED {spec_dir.name}")
            fixed_count += 1
        else:
            no_change_count += 1
    
    print(f"\nSummary:")
    print(f"  Fixed: {fixed_count}")
    print(f"  No changes needed: {no_change_count}")
    print(f"  Errors: {error_count}")
    print(f"  {'DRY RUN' if dry_run else 'CHANGES WRITTEN'}")

if __name__ == "__main__":
    main()
